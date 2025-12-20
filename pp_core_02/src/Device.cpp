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

uint64_t Device::acceptMicros = 0;
uint64_t Device::acceptCount = 0;

bool Device::begin() {
    return true;
}

void Device::reset(double x, double y) {

    // calculate current position
    coord_corexy_____t curCorexy = Motors::getCurCorexy();
    coord_planxy_d___t curPlanxy = Coords::corexyToPlanxy(curCorexy);

    // adjust current position
    curPlanxy = {x, y, curPlanxy.z};
    curCorexy = Coords::planxyToCorexy(curPlanxy);

    // apply counters to motors A and B
    Motors::motorA.setCntrCur(curCorexy.a);
    Motors::motorB.setCntrCur(curCorexy.b);
}

void Device::yield() {
    Driver::yield();
    Device::motorPrim = nullptr;
    Device::motorSec1 = nullptr;
    Device::motorSec2 = nullptr;
}

/**
 * called from Pulse::pulse() on CPU-0
 */
void Device::pulse() {

    // TODO :: limit switches

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
            if (Coords::hasBlock()) {          // check if there is more blocks to be handled at the moment
                Device::accept(Coords::popBlock());
            } else {
                Device::yield();
            }
        } else {
            if (Device::frqI != Device::frqO) {
                // v²=v0²+2as // https://www.studysmarter.de/schule/physik/mechanik/gleichmaessig-beschleunigte-bewegung/
                double frequencyC = sqrt(Device::frqA2 * Device::cPrim + Device::frqII);
                Driver::setFrequency(frequencyC);
            }
        }

    } else if (Coords::hasBlock()) {
        Device::accept(Coords::popBlock());
    }
}

bool Device::accept(block_planxy_d___t dstPlanxy) {

    uint64_t acceptMicrosA = micros();

    if (dstPlanxy.z == VALUE______RESET) {

        Device::reset(dstPlanxy.x, dstPlanxy.y);
        Device::homedZ = false;

        // will cause up movement until z-switch is pressed
        dstPlanxy.x = 0;
        dstPlanxy.y = 0;
        dstPlanxy.z = 10;

        // with the very high z-value, normal acceleration calc would not work, therefore setting default velocities
        dstPlanxy.vi = MACHINE_HOME_V_Z;
        dstPlanxy.vo = MACHINE_HOME_V_Z;
    }

    // TODO :: check if this case still occurs and maybe handle in another way
    if (dstPlanxy.vi == 0 && dstPlanxy.vo == 0) {
        dstPlanxy.vi = MACHINE_HOME_VXY;
        dstPlanxy.vo = MACHINE_HOME_VXY;
    }

    // src coordinates
    coord_corexy_____t srcCorexy = Motors::getCurCorexy();             // where the machine is in terms of motor counters
    coord_planxy_d___t srcPlanxy = Coords::corexyToPlanxy(srcCorexy);  // where the machine is in planar space

    // destination coordinate
    coord_corexy_____t dstCorexy = Coords::planxyToCorexy(dstPlanxy);  // where the machine needs to go in planar space

    // vectors to reach destination
    coord_corexy_____t vecCorexy = Coords::toCorexyVector(srcCorexy, dstCorexy);
    coord_planxy_d___t vecPlanxy = Coords::toPlanxyVector(srcPlanxy, dstPlanxy);

    // ~ 19 microseconds

    // planar distance to reach destination
    double lengthPlanxy = sqrt(vecPlanxy.x * vecPlanxy.x + vecPlanxy.y * vecPlanxy.y + vecPlanxy.z * vecPlanxy.z);  // Coords::toLength(vecPlanxy);

    // ~ 26 microseconds

    // duration to reach destination (each block needs to have linear acceleration or constant speed)
    uint64_t microsTotal = lengthPlanxy * 2 * MICROSECONDS_PER_SECOND / (dstPlanxy.vi + dstPlanxy.vo);

    // ~ 29 microseconds

    uint32_t baseStepsA = abs(vecCorexy.a);
    uint32_t baseStepsB = abs(vecCorexy.b);
    uint32_t baseStepsZ = abs(vecCorexy.z);

    // seconds = length(mm) / speed(mm/s), inverse value used here
    double maxVMult = lengthPlanxy != 0.0 ? max(dstPlanxy.vi, dstPlanxy.vo) / lengthPlanxy : 0;

    // frequency(Hz) = steps / seconds, i.e. 4000/2 = 2000Hz, inverse value used to replace division with multiplication
    // TODO :: maybe frequency can be replaced with alarmValue - harder to read, but fixed point math
    double baseFrequencyA = baseStepsA * maxVMult;
    double baseFrequencyB = baseStepsB * maxVMult;
    double baseFrequencyZ = baseStepsZ * maxVMult;

    // ~ 35 microseconds

    motor_settings___t motorSettingsA = {PIN_STATUS__LOW, 1, Motors::motorA.findMicrostepSettings(baseFrequencyA)};
    motor_settings___t motorSettingsB = {PIN_STATUS__LOW, 1, Motors::motorB.findMicrostepSettings(baseFrequencyB)};
    motor_settings___t motorSettingsZ = {PIN_STATUS__LOW, 1, Motors::motorZ.findMicrostepSettings(baseFrequencyZ)};

    // ~ 38 microseconds

    if (vecCorexy.a < 0) {
        motorSettingsA.counterIncrement = -1;
        motorSettingsA.directVal = PIN_STATUS_HIGH;
    }
    if (vecCorexy.b < 0) {
        motorSettingsB.counterIncrement = -1;
        motorSettingsB.directVal = PIN_STATUS_HIGH;
    }
    if (vecCorexy.z < 0) {
        motorSettingsZ.counterIncrement = -1;
        motorSettingsZ.directVal = PIN_STATUS_HIGH;
    }

    if (dstPlanxy.z == VALUE______RESET) {

        Device::reset(dstPlanxy.x, dstPlanxy.y);
        Device::homedZ = false;

        // will cause up movement until z-switch is pressed
        dstPlanxy.x = 0;
        dstPlanxy.y = 0;
        dstPlanxy.z = 10;
    }

    if (vecCorexy.a != 0.0 || vecCorexy.b != 0.0 || vecCorexy.z != 0.0) {

        uint32_t stepsA = baseStepsA * motorSettingsA.settingsMicro.microMlt;
        uint32_t stepsB = baseStepsB * motorSettingsB.settingsMicro.microMlt;
        uint32_t stepsZ = baseStepsZ * motorSettingsZ.settingsMicro.microMlt;

        // Serial.print("stepsA: ");
        // Serial.print(String(stepsA));
        // Serial.print(", stepsB: ");
        // Serial.print(String(stepsB));
        // Serial.print(", stepsZ: ");
        // Serial.print(String(stepsZ));
        // Serial.print(", dir: ");

        uint32_t maxSteps = max({stepsA, stepsB, stepsZ});

        if (stepsA == maxSteps) {
            Device::motorPrim = &Motors::motorA;
            Device::motorSec1 = &Motors::motorB;
            Device::motorSec2 = &Motors::motorZ;
            Device::dPrim = stepsA;
            Device::dSec1 = stepsB;
            Device::dSec2 = stepsZ;
            // Serial.print("ABZ");
        } else if (stepsB == maxSteps) {
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

        // ~ 41 microseconds

        // helper values for adjusting frequencies later on
        Device::frqI = Device::dPrim * dstPlanxy.vi / lengthPlanxy;
        Device::frqO = Device::dPrim * dstPlanxy.vo / lengthPlanxy;
        Device::frqA2 = (Device::frqO - Device::frqI) * 2000000.0 / microsTotal;
        Device::frqII = Device::frqI * Device::frqI;

        // more bresenham algorithm values
        Device::cPrim = 0;
        Device::eSec1 = 2 * Device::dSec1 - Device::dPrim;
        Device::eSec2 = 2 * Device::dSec2 - Device::dPrim;

        // ~ 52 microseconds

        // point the motors in the proper direction
        Motors::motorA.applySettings(motorSettingsA);
        Motors::motorB.applySettings(motorSettingsB);
        Motors::motorZ.applySettings(motorSettingsZ);

        // ~ 59 microseconds

        Driver::setFrequency(Device::frqI);

        Device::acceptMicros += (micros() - acceptMicrosA);
        Device::acceptCount++;
        // ~ 67 microseconds

        return true;

    } else {
        // Serial.println("ignoring all a-b-z zero block");
        return false;
    }
}
