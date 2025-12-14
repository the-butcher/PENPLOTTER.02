#include "Device.h"

double Device::frqI = 1.0;
double Device::frqO = 1.0;
double Device::frqA2 = 0.0;
double Device::frqII = 1.0;

Motor* Device::motorPrim = nullptr;
Motor* Device::motorSec1 = nullptr;
Motor* Device::motorSec2 = nullptr;

uint32_t Device::cPrim = 0;
uint32_t Device::dPrim = 0;
uint32_t Device::dSec1 = 0;
uint32_t Device::dSec2 = 0;
int32_t Device::eSec1 = 0;
int32_t Device::eSec2 = 0;

bool Device::homedX = false;
bool Device::homedY = false;
bool Device::homedZ = false;

bool Device::begin() {
    return true;
}

void Device::reset(double x, double y) {

    // calculate current position
    coord_corexy_____t curCorexy = Motors::getCurCorexy();
    coord_planxy_____t curPlanxy = Coords::corexyToPlanxy(curCorexy);

    // adjust current position
    curPlanxy = {x, y, curPlanxy.z};
    curCorexy = Coords::planxyToCorexy(curPlanxy);

    // apply counters to motors A and B
    Motors::motorA.setCntrCur(curCorexy.a);
    Motors::motorB.setCntrCur(curCorexy.b);
}

void Device::yield() {
    Pulse::yield();
    Device::motorPrim = nullptr;
    Device::motorSec1 = nullptr;
    Device::motorSec2 = nullptr;
}

/**
 * called from Pulse::pulse() on CPU-0
 */
void Device::pulse() {

    // TODO :: switches

    // exectute a single bresenham step, https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    if (Device::motorPrim != nullptr) {

        Device::motorPrim->pulse();
        if (eSec1 > 0) {
            Device::motorSec1->pulse();
            Device::eSec1 = Device::eSec1 - 2 * Device::dPrim;
        }
        if (eSec2 > 0) {
            Device::motorSec2->pulse();
            Device::eSec2 = Device::eSec2 - 2 * Device::dPrim;
        }
        Device::eSec1 = Device::eSec1 + 2 * Device::dSec1;
        Device::eSec2 = Device::eSec2 + 2 * Device::dSec2;

        Device::cPrim++;
        if (Device::cPrim >= Device::dPrim) {  // counter has reached the segment delta
            // TODO :: segment complete, find new destination coordinate
            if (Coords::hasBlock()) {  // check if there is more blocks to be handled at the moment
                // Serial.println("-- block :: coords --> device (pulse1) --------------------------------");
                block_device_____t currBlock = Coords::popBlock();  // TODO :: revisit bluetooth implementation to verify that indices for picking a block never get corrupted by appending blocks
                Device::accept(currBlock);
            } else {
                Device::yield();
            }
        } else {
            if (Device::frqI != Device::frqO) {
                double frequencyC = sqrt(Device::frqA2 * Device::cPrim + Device::frqII);
                Pulse::setFrequency(frequencyC);
            }
        }

    } else if (Coords::hasBlock()) {
        // Serial.println("-- block :: coords --> device (pulse2) --------------------------------");
        block_device_____t dstDevice = Coords::popBlock();
        Device::accept(dstDevice);
    }
}

bool Device::accept(block_device_____t& dstDevice) {

    block_planxy_____t dstPlanxy = dstDevice.dstPlanxy;
    // coord_corexy_____t vecCorexy = dstDevice.vecCorexy;

    coord_corexy_____t srcCorexy = Motors::getCurCorexy();
    coord_corexy_____t dstCorexy = Coords::planxyToCorexy(dstPlanxy);
    coord_corexy_____t vecCorexy = Coords::toCorexyVector(srcCorexy, dstCorexy);

    if (vecCorexy.a != dstDevice.vecCorexy.a || vecCorexy.b != dstDevice.vecCorexy.b || vecCorexy.z != dstDevice.vecCorexy.z) {
        Serial.println("vecCorexy inconsistency: ");
        if (vecCorexy.a != dstDevice.vecCorexy.a) {
            Serial.print(vecCorexy.a);
            Serial.print(" a ");
            Serial.println(dstDevice.vecCorexy.a);
        }
        if (vecCorexy.b != dstDevice.vecCorexy.b) {
            Serial.print(vecCorexy.b);
            Serial.print(" b ");
            Serial.println(dstDevice.vecCorexy.b);
        }
        coord_planxy_____t srcPlanxy = Coords::corexyToPlanxy(srcCorexy);
        dstDevice = Coords::toDeviceBlock(srcPlanxy, dstPlanxy);
    } else {
        // Serial.println("vecCorexy consistent");
    }

    if (dstPlanxy.z == VALUE______RESET) {

        Device::reset(dstPlanxy.x, dstPlanxy.y);
        Device::homedZ = false;

        // will cause up movement until z-switch is pressed
        dstPlanxy.x = 0;
        dstPlanxy.y = 0;
        dstPlanxy.z = 10;

        // TODO :: if there are any further coordinates in buffer, these likely need to be recalculated!!
    }

    if (vecCorexy.a != 0.0 || vecCorexy.b != 0.0 || vecCorexy.z != 0.0) {

        // TODO :: much of the stuff below could be moved to the device block too
        // TODO :: check if vecCorexy inconsistency is a thing
        // TODO :: check if there are memory concerns

        uint32_t stepsA = abs(vecCorexy.a) * dstDevice.settingsA.settingsMicro.micrMlt;
        uint32_t stepsB = abs(vecCorexy.b) * dstDevice.settingsB.settingsMicro.micrMlt;
        uint32_t stepsZ = abs(vecCorexy.z) * dstDevice.settingsZ.settingsMicro.micrMlt;

        // Serial.print("stepsA: ");
        // Serial.print(String(stepsA));
        // Serial.print(", stepsB: ");
        // Serial.print(String(stepsB));
        // Serial.print(", stepsZ: ");
        // Serial.print(String(stepsZ));
        // Serial.print(", dir: ");

        if (stepsA >= max(stepsB, stepsZ)) {
            Device::motorPrim = &Motors::motorA;
            Device::motorSec1 = &Motors::motorB;
            Device::motorSec2 = &Motors::motorZ;
            Device::dPrim = stepsA;
            Device::dSec1 = stepsB;
            Device::dSec2 = stepsZ;
            // Serial.print("ABZ");
        } else if (stepsB >= max(stepsA, stepsZ)) {
            Device::motorPrim = &Motors::motorB;
            Device::motorSec1 = &Motors::motorA;
            Device::motorSec2 = &Motors::motorZ;
            Device::dPrim = stepsB;
            Device::dSec1 = stepsA;
            Device::dSec2 = stepsZ;
            // Serial.print("BAZ");
        } else {
            Device::motorPrim = &Motors::motorZ;
            Device::motorSec1 = &Motors::motorA;
            Device::motorSec2 = &Motors::motorB;
            Device::dPrim = stepsZ;
            Device::dSec1 = stepsA;
            Device::dSec2 = stepsB;
            // Serial.print("ZAB");
        }
        // Serial.println("");

        // helper values for adjusting frequencies later on
        Device::frqI = Device::dPrim * dstPlanxy.vi / dstDevice.lenPlanxy;
        Device::frqO = Device::dPrim * dstPlanxy.vo / dstDevice.lenPlanxy;
        Device::frqA2 = (Device::frqO - Device::frqI) * 2000000.0 / dstDevice.microsTotal;
        Device::frqII = Device::frqI * Device::frqI;

        // more bresenham algorithm values
        Device::cPrim = 0;
        Device::eSec1 = 2 * Device::dSec1 - Device::dPrim;
        Device::eSec2 = 2 * Device::dSec2 - Device::dPrim;

        // point the motors in the proper direction
        Motors::motorA.applySettings(dstDevice.settingsA);
        Motors::motorB.applySettings(dstDevice.settingsB);
        Motors::motorZ.applySettings(dstDevice.settingsZ);

        Pulse::setFrequency(Device::frqI);

        return true;

    } else {
        // Serial.println("ignoring all a-b-z zero block");
        return false;
    }
}
