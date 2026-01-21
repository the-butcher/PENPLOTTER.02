#include "Device.h"

int64_t Device::frqI_mHz = 1L;
int64_t Device::frqO_mHz = 1L;
int64_t Device::frqA2 = 0L;
int64_t Device::frqII = 1L;

uint64_t Device::lenP__um = 0L;
int64_t Device::durP__us = 0L;

Motor* Device::motorPrim = nullptr;
Motor* Device::motorSec1 = nullptr;
Motor* Device::motorSec2 = nullptr;
Motor* Device::motorSec3 = nullptr;

uint32_t Device::cPrim = 0;
uint32_t Device::dPrim = 0;
uint32_t Device::dSec1 = 0;
uint32_t Device::dSec2 = 0;
uint32_t Device::dSec3 = 0;
int32_t Device::eSec1 = 0;
int32_t Device::eSec2 = 0;
int32_t Device::eSec3 = 0;

bool Device::homedX = false;
bool Device::homedY = false;
bool Device::homedZ = false;

uint64_t Device::acceptMicros = 0L;
uint64_t Device::acceptCount = 0L;

bool Device::begin() {
    return true;
}

void Device::reset(int64_t x_um, int64_t y_um) {

    // calculate current position
    coord_corexy_____t curCorexy = Motors::getCurCorexy();
    coord_planxy_i64_t curPlanxy = Coords::corexyToPlanxy(curCorexy);

    // adjust current position
    curPlanxy = {x_um, y_um, curPlanxy.z};
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
    Device::motorSec3 = nullptr;
}

/**
 * called from Pulse::pulse() on CPU-0
 */
void Device::pulse() {

    Switches::readAll();

    if (Switches::limitX.isPressed()) {
        if (!Device::homedX) {
            Device::yield();  // will set all motors to nullptr
            Device::homedX = true;
            Device::reset(-10000L, 0L);  // treat this location as -10mm/0mm
        } else {
            // TODO :: maybe implement another threshold as yield criteria
        }
    }

    if (Switches::limitY.isPressed()) {
        if (!Device::homedY) {
            Device::yield();
            Device::homedY = true;
            Device::reset(0L, -20000L);  // treat this location as 0mm/-20mm, TODO :: use current x value instead of 0
        } else {
            // TODO :: maybe implement another threshold as yield criteria
        }
    }

    if (Switches::limitZ.isPressed()) {
        if (!Device::homedZ) {
            Device::yield();  // will set all motors to nullptr
            Device::homedZ = true;
            Motors::motorZ.setCntrCur(0);
        } else {
            // TODO :: maybe implement another threshold as yield criteria
        }
    }

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
        if (eSec3 > 0) {
            Device::motorSec3->pulse();
            Device::eSec3 = Device::eSec3 - 2 * Device::dPrim;
        }
        Device::eSec1 = Device::eSec1 + 2 * Device::dSec1;
        Device::eSec2 = Device::eSec2 + 2 * Device::dSec2;
        Device::eSec3 = Device::eSec3 + 2 * Device::dSec3;

        Device::cPrim++;
        if (Device::cPrim >= Device::dPrim) {  // counter has reached the segment delta
            if (Coords::hasBlock()) {          // check if there is more blocks to be handled at the moment
                Device::accept(Coords::popBlock());
            } else {
                Device::yield();
            }
        } else {
            if (Device::frqI_mHz != Device::frqO_mHz) {
                // v²=v0²+2as // https://www.studysmarter.de/schule/physik/mechanik/gleichmaessig-beschleunigte-bewegung/
                uint64_t frq___mHz = sqrt(Device::frqA2 * Device::cPrim * 1000L + Device::frqII);
                Driver::setFrq___mHz(frq___mHz);
            }
        }

    } else if (Coords::hasBlock()) {
        Device::accept(Coords::popBlock());
    }
}

bool Device::accept(block_planxy_i64_t dstPlanxy) {

#ifdef USE____DEBUG
    Serial.print("dstPlanxy, x: ");
    Serial.print(String(dstPlanxy.x));
    Serial.print(", y: ");
    Serial.print(String(dstPlanxy.y));
    Serial.print(", z: ");
    Serial.print(String(dstPlanxy.z));
    Serial.print(", r: ");
    Serial.print(String(dstPlanxy.r));
    Serial.print(", vi: ");
    Serial.print(String(dstPlanxy.vi));
    Serial.print(", vo: ");
    Serial.println(String(dstPlanxy.vo));
#endif

    uint64_t acceptMicrosA = micros();

    // if (dstPlanxy.z == VALUE______RESET) {

    //     Device::reset(dstPlanxy.x, dstPlanxy.y);
    //     Device::homedZ = false;

    //     // will cause up movement until z-switch is pressed
    //     dstPlanxy.x = 0;
    //     dstPlanxy.y = 0;
    //     dstPlanxy.z = 10;

    //     // with the very high z-value, normal acceleration calc would not work, therefore setting default velocities
    //     dstPlanxy.vi = MACHINE_HOME_V_Z;
    //     dstPlanxy.vo = MACHINE_HOME_V_Z;
    // }

    // TODO :: check if this case still occurs and maybe handle in another way
    if (dstPlanxy.vi == 0 && dstPlanxy.vo == 0) {
        dstPlanxy.vi = DEVICE___V_XY_ums;
        dstPlanxy.vo = DEVICE___V_XY_ums;
    } else if (dstPlanxy.vi == 0) {
        dstPlanxy.vi = DEVICE___V_XY_ums;
    } else if (dstPlanxy.vo == 0) {
        dstPlanxy.vo = DEVICE___V_XY_ums;
    }

    // src coordinates
    coord_corexy_____t srcCorexy = Motors::getCurCorexy();             // where the machine is in terms of motor counters
    coord_planxy_i64_t srcPlanxy = Coords::corexyToPlanxy(srcCorexy);  // where the machine is in planar space

    // destination coordinate
    coord_corexy_____t dstCorexy = Coords::planxyToCorexy(dstPlanxy);  // where the machine needs to go in planar space

    // vectors to reach destination
    coord_corexy_____t vecCorexy = Coords::toCorexyVector(srcCorexy, dstCorexy);
    coord_planxy_i64_t vecPlanxy = Coords::toPlanxyVector(srcPlanxy, dstPlanxy);

#ifdef USE____DEBUG
    Serial.print("vecCorexy, x: ");
    Serial.print(String(vecCorexy.a));
    Serial.print(", y: ");
    Serial.print(String(vecCorexy.b));
    Serial.print(", z: ");
    Serial.print(String(vecCorexy.z));
    Serial.print(", r: ");
    Serial.println(String(vecCorexy.r));
    Serial.print("vecPlanxy, x: ");
    Serial.print(String(vecPlanxy.x));
    Serial.print(", y: ");
    Serial.print(String(vecPlanxy.y));
    Serial.print(", z: ");
    Serial.print(String(vecPlanxy.z));
    Serial.print(", r: ");
    Serial.println(String(vecPlanxy.r));
#endif

    // ~ 9 microseconds

    Device::lenP__um = Coords::toPlanLength(vecPlanxy);  // planar distance to reach destination
    uint64_t lenR__ud = abs(vecPlanxy.r);                // rotational distance in microdegrees

#ifdef USE____DEBUG
    Serial.print("lenP__um: ");
    Serial.println(String(Device::lenP__um));
#endif

    // ~ 14 microseconds

    // duration to reach destination (each block needs to have linear acceleration or constant speed)
    Device::durP__us = Device::lenP__um * TWO_SECONDS____us / (dstPlanxy.vi + dstPlanxy.vo);
    int64_t durD__us = lenR__ud * ONE_SECOND_____us / DEVICE___V__R_uds;  // the minimum duration for the required rotation to complete

#ifdef USE____DEBUG
    Serial.print("durP__us: ");
    Serial.println(String(Device::durP__us));
    Serial.print("durD__us: ");
    Serial.println(String(durD__us));
#endif

    if (Device::durP__us == 0) {  // rotation only

        dstPlanxy.vi = DEVICE___V__R_uds;  // max rotation speed
        dstPlanxy.vo = DEVICE___V__R_uds;
        Device::lenP__um = lenR__ud;
        Device::durP__us = durD__us;

#ifdef USE____DEBUG
        Serial.print("vi (new): ");
        Serial.print(String(dstPlanxy.vi));
        Serial.print(", vo (new): ");
        Serial.println(String(dstPlanxy.vo));
        Serial.print("durD__us (new): ");
        Serial.println(String(durD__us));
#endif

    } else if (durD__us > Device::durP__us) {  // rotation requires more time than the planar distance, speed and duration adaption required

        dstPlanxy.vi = dstPlanxy.vi * Device::durP__us / durD__us;
        dstPlanxy.vo = dstPlanxy.vo * Device::durP__us / durD__us;

        Device::durP__us = Device::lenP__um * TWO_SECONDS____us / (dstPlanxy.vi + dstPlanxy.vo);

#ifdef USE____DEBUG
        Serial.print("vi (new): ");
        Serial.print(String(dstPlanxy.vi));
        Serial.print(", vo (new): ");
        Serial.println(String(dstPlanxy.vo));
        Serial.print("durD__us (new): ");
        Serial.println(String(durD__us));
#endif
    }

    // ~ 15 microseconds

    uint32_t baseStepsA = abs(vecCorexy.a);
    uint32_t baseStepsB = abs(vecCorexy.b);
    uint32_t baseStepsZ = abs(vecCorexy.z);
    uint32_t baseStepsR = abs(vecCorexy.r);

#ifdef USE____DEBUG
    Serial.print("baseStepsA: ");
    Serial.println(String(baseStepsA));
    Serial.print("baseStepsB: ");
    Serial.println(String(baseStepsB));
    Serial.print("baseStepsZ: ");
    Serial.println(String(baseStepsZ));
    Serial.print("baseStepsR: ");
    Serial.println(String(baseStepsR));
#endif

    // seconds = length(mm) / speed(mm/s), inverse value used here
    uint64_t maxVMult2 = Device::lenP__um > 0 ? max(dstPlanxy.vi, dstPlanxy.vo) * 1000000L / Device::lenP__um : 0;

#ifdef USE____DEBUG
    Serial.print("maxVMult2: ");
    Serial.println(String(maxVMult2));
#endif

    // frequency(Hz) = steps / seconds, i.e. 4000/2 = 2000Hz, inverse value used to replace division with multiplication
    uint64_t frqA_mHz = baseStepsA * maxVMult2 / 1000L;
    uint64_t frqB_mHz = baseStepsB * maxVMult2 / 1000L;
    uint64_t frqZ_mHz = baseStepsZ * maxVMult2 / 1000L;
    uint64_t frqR_mHz = baseStepsR * maxVMult2 / 1000L;

#ifdef USE____DEBUG
    Serial.print("frqA__mHz: ");
    Serial.println(String(frqA_mHz));
    Serial.print("frqB__mHz: ");
    Serial.println(String(frqB_mHz));
    Serial.print("frqZ__mHz: ");
    Serial.println(String(frqZ_mHz));
    Serial.print("frqR__mHz: ");
    Serial.println(String(frqR_mHz));
#endif

    // ~ 16 microseconds

    motor_settings___t motorSettingsA = {PIN_STATUS__LOW, 1, Motors::motorA.findMicrostepSettings(frqA_mHz)};
    motor_settings___t motorSettingsB = {PIN_STATUS__LOW, 1, Motors::motorB.findMicrostepSettings(frqB_mHz)};
    motor_settings___t motorSettingsZ = {PIN_STATUS__LOW, 1, Motors::motorZ.findMicrostepSettings(frqZ_mHz)};
    motor_settings___t motorSettingsR = {PIN_STATUS__LOW, 1, Motors::motorR.findMicrostepSettings(frqR_mHz)};

#ifdef USE____DEBUG
    Serial.print("motorSettingsA x");
    Serial.println(String(motorSettingsA.settingsMicro.microMlt));
    Serial.print("motorSettingsB x");
    Serial.println(String(motorSettingsB.settingsMicro.microMlt));
    Serial.print("motorSettingsZ x");
    Serial.println(String(motorSettingsZ.settingsMicro.microMlt));
    Serial.print("motorSettingsR x");
    Serial.println(String(motorSettingsR.settingsMicro.microMlt));
#endif

    // ~ 20 microseconds

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
    if (vecCorexy.r < 0) {
        motorSettingsR.counterIncrement = -1;
        motorSettingsR.directVal = PIN_STATUS_HIGH;
    }

    if (dstPlanxy.z == VALUE______RESET) {

        Device::reset(dstPlanxy.x, dstPlanxy.y);
        Device::homedZ = false;

        // will cause up movement until z-switch is pressed
        dstPlanxy.x = 0;
        dstPlanxy.y = 0;
        dstPlanxy.z = 10;
    }

    if (vecCorexy.a != 0.0 || vecCorexy.b != 0.0 || vecCorexy.z != 0.0 || vecCorexy.r != 0.0) {

        uint32_t stepsA = baseStepsA * motorSettingsA.settingsMicro.microMlt;
        uint32_t stepsB = baseStepsB * motorSettingsB.settingsMicro.microMlt;
        uint32_t stepsZ = baseStepsZ * motorSettingsZ.settingsMicro.microMlt;
        uint32_t stepsR = baseStepsR * motorSettingsR.settingsMicro.microMlt;

#ifdef USE____DEBUG
        Serial.print("stepsA: ");
        Serial.println(String(stepsA));
        Serial.print("stepsB: ");
        Serial.println(String(stepsB));
        Serial.print("stepsZ: ");
        Serial.println(String(stepsZ));
        Serial.print("stepsR: ");
        Serial.println(String(stepsR));
#endif

        uint32_t maxSteps = max({stepsA, stepsB, stepsZ, stepsR});
        if (stepsA == maxSteps) {
            Device::motorPrim = &Motors::motorA;
            Device::motorSec1 = &Motors::motorB;
            Device::motorSec2 = &Motors::motorZ;
            Device::motorSec3 = &Motors::motorR;
            Device::dPrim = stepsA;
            Device::dSec1 = stepsB;
            Device::dSec2 = stepsZ;
            Device::dSec3 = stepsR;
            // Serial.print("ABZR");
        } else if (stepsB == maxSteps) {
            Device::motorPrim = &Motors::motorB;
            Device::motorSec1 = &Motors::motorA;
            Device::motorSec2 = &Motors::motorZ;
            Device::motorSec3 = &Motors::motorR;
            Device::dPrim = stepsB;
            Device::dSec1 = stepsA;
            Device::dSec2 = stepsZ;
            Device::dSec3 = stepsR;
            // Serial.print("BAZR");
        } else if (stepsZ == maxSteps) {
            Device::motorPrim = &Motors::motorZ;
            Device::motorSec1 = &Motors::motorA;
            Device::motorSec2 = &Motors::motorB;
            Device::motorSec3 = &Motors::motorR;
            Device::dPrim = stepsZ;
            Device::dSec1 = stepsA;
            Device::dSec2 = stepsB;
            Device::dSec3 = stepsR;
            // Serial.print("ZABR");
        } else {
            Device::motorPrim = &Motors::motorR;
            Device::motorSec1 = &Motors::motorA;
            Device::motorSec2 = &Motors::motorB;
            Device::motorSec3 = &Motors::motorZ;
            Device::dPrim = stepsR;
            Device::dSec1 = stepsA;
            Device::dSec2 = stepsB;
            Device::dSec3 = stepsZ;
            // Serial.print("RABZ");
        }
        // Serial.println("");

        // ~ 23 microseconds

        // helper values for adjusting frequencies later on
        Device::frqI_mHz = Device::dPrim * dstPlanxy.vi * 1000L / Device::lenP__um;
        Device::frqO_mHz = Device::dPrim * dstPlanxy.vo * 1000L / Device::lenP__um;
        Device::frqA2 = (Device::frqO_mHz - Device::frqI_mHz) * 2000000L / Device::durP__us;  // this should probably be milliseconds, would likely save multiplication by 1000L in pulse()
        Device::frqII = Device::frqI_mHz * Device::frqI_mHz;

#ifdef USE____DEBUG
        // frqI_mHz: 1114948
        // frqO_mHz: 4459795
        Serial.print("frqI_mHz: ");
        Serial.println(String(Device::frqI_mHz));
        Serial.print("frqO_mHz: ");
        Serial.println(String(Device::frqO_mHz));
        Serial.print("frqA2: ");
        Serial.println(String(Device::frqA2));
        Serial.print("frqII: ");
        Serial.println(String(Device::frqII));
        Serial.print("dPrim: ");
        Serial.println(String(Device::dPrim));
#endif

        // more bresenham algorithm values
        Device::cPrim = 0;
        Device::eSec1 = 2 * Device::dSec1 - Device::dPrim;
        Device::eSec2 = 2 * Device::dSec2 - Device::dPrim;
        Device::eSec3 = 2 * Device::dSec3 - Device::dPrim;

        // ~ 26 microseconds

        // point the motors in the proper direction
        Motors::motorA.applySettings(motorSettingsA);
        Motors::motorB.applySettings(motorSettingsB);
        Motors::motorZ.applySettings(motorSettingsZ);
        Motors::motorR.applySettings(motorSettingsR);

        // ~ 33 microseconds

        Driver::setFrq___mHz(Device::frqI_mHz);

        Device::acceptMicros += (micros() - acceptMicrosA);
        Device::acceptCount++;
        // ~ 36 microseconds

        return true;

    } else {
        // Serial.println("ignoring all a-b-z zero block");
        return false;
    }
}
