#include "Coords.h"

block_device_____t Coords::blockBufferDevice[BLOCK_BUFFER_SIZE];

uint32_t Coords::nextBlockIndex = 0;
uint32_t Coords::blockIndex = 0;

bool Coords::begin() {
    // Coords::accept({0.0, 0.0, VALUE______RESET, MACHINE_HOME_V_Z, MACHINE_HOME_V_Z});   // move until z-swtich touched, z-homing the machine and accepting the z-home coordinate as 0.0
    // Coords::accept({-VALUE______RESET, 0.0, 0.0, MACHINE_HOME_VXY, MACHINE_HOME_VXY});  // move until x-switch touched, x-homing the machine and accepting the limit x-coordinate as -10.0
    // Coords::accept({0.0, 0.0, 0.0, MACHINE_HOME_VXY, MACHINE_HOME_VXY});                // backup x to 0.0
    // Coords::accept({0.0, -VALUE______RESET, 0.0, MACHINE_HOME_VXY, MACHINE_HOME_VXY});  // move until y-swtich touched, y-homing the machine and accepting the limit y-coordinate as -20.0
    // Coords::accept({0.0, 0.0, 0.0, MACHINE_HOME_VXY, MACHINE_HOME_VXY});                // backup y to 0.0, the machine will now be at x0.0, y0.0 with a lifted pen at z0.0

    // stores a first set of blocks to the buffers (no need to check for prev index in Coords::accept)
    // Serial.println("-- block :: begin  --> coords -----------------------------------------");
    Coords::blockBufferDevice[Coords::blockIndex % BLOCK_BUFFER_SIZE] = {{0, 0, 0}, {0, 0, 0}, {PIN_STATUS__LOW, 1, Motors::motorA.findMicrostepSettings(0)}, {PIN_STATUS__LOW, 1, Motors::motorB.findMicrostepSettings(0)}, {PIN_STATUS__LOW, 1, Motors::motorZ.findMicrostepSettings(0)}};
    Coords::blockIndex++;

    // Serial.println("-- block :: begin  --> coords -----------------------------------------");
    for (double r = 0; r < PI * 2; r += PI / 180) {
        Coords::addBlock({sin(r) * 100, cos(r) * 100 - 100, 0, 30, 30});
    }

    return true;
}

bool Coords::hasBlock() {
    return Coords::nextBlockIndex < Coords::blockIndex;
}

block_device_____t Coords::popBlock() {
    return Coords::blockBufferDevice[Coords::nextBlockIndex++ % BLOCK_BUFFER_SIZE];
}

block_device_____t Coords::toDeviceBlock(coord_planxy_____t& srcPlanxy, block_planxy_____t& dstPlanxy) {

    if (dstPlanxy.z == VALUE______RESET) {
        // TODO :: make sure there is a reset to the xy position in this coordinate
        // Machine::reset(blockPlanxy.x, blockPlanxy.y);
        // Machine::homedZ = false;

        // will cause up movement until z-switch is pressed
        // TODO :: limit z-movement to device z-range (?), actually the reset value will make it travel until the z-switch is pressed
        // with the very high z-value, normal acceleration calc would not work, therefore setting default velocities
        dstPlanxy.vi = MACHINE_HOME_V_Z;
        dstPlanxy.vo = MACHINE_HOME_V_Z;

        // TODO :: think about reference coord
        // TODO :: what needs to happen with the block-buffer (especially at machine init) in case of any reset, xy-reset or z-reset
    }

    // TODO :: check if this case still occurs and maybe handle in another way
    if (dstPlanxy.vi == 0 && dstPlanxy.vo == 0) {
        dstPlanxy.vi = MACHINE_HOME_VXY;
        dstPlanxy.vo = MACHINE_HOME_VXY;
    }

    // src coordinates (TODO :: make sure assuming a specific src-coordinate does not introduce some kind of drift as potential step error accumulates)
    coord_corexy_____t srcCorexy = Coords::planxyToCorexy(srcPlanxy);

    // destination coordinate
    coord_corexy_____t dstCorexy = Coords::planxyToCorexy(dstPlanxy);

    // vectors to reach destination
    coord_corexy_____t vecCorexy = Coords::toCorexyVector(srcCorexy, dstCorexy);
    coord_planxy_____t vecPlanxy = Coords::toPlanxyVector(srcPlanxy, dstPlanxy);

    // skip zero length commands
    // if (vecCorexy.a != 0.0 || vecCorexy.b != 0.0 || vecCorexy.z != 0.0) {

    // planar distance to reach destination
    double lenPlanxy = Coords::toLength(vecPlanxy);

    // duration to reach destination (each block needs to have linear acceleration or constant speed)
    uint64_t microsTotal = lenPlanxy * 2 * MICROSECONDS_PER_SECOND / (dstPlanxy.vi + dstPlanxy.vo);  // total microseconds

    uint32_t stepsA = abs(vecCorexy.a);
    uint32_t stepsB = abs(vecCorexy.b);
    uint32_t stepsZ = abs(vecCorexy.z);

    double maxVMult = lenPlanxy != 0.0 ? max(dstPlanxy.vi, dstPlanxy.vo) / lenPlanxy : 0;
    double frqA = stepsA * maxVMult;
    double frqB = stepsB * maxVMult;
    double frqZ = stepsZ * maxVMult;

    motor_settings___t motorSettingsA = {PIN_STATUS__LOW, 1, Motors::motorA.findMicrostepSettings(frqA)};
    motor_settings___t motorSettingsB = {PIN_STATUS__LOW, 1, Motors::motorB.findMicrostepSettings(frqB)};
    motor_settings___t motorSettingsZ = {PIN_STATUS__LOW, 1, Motors::motorZ.findMicrostepSettings(frqZ)};

    if (vecCorexy.a < 0) {
        motorSettingsA.cntrInc = -1;
        motorSettingsA.drctVal = PIN_STATUS_HIGH;
    }
    if (vecCorexy.b < 0) {
        motorSettingsB.cntrInc = -1;
        motorSettingsB.drctVal = PIN_STATUS_HIGH;
    }
    if (vecCorexy.z < 0) {
        motorSettingsZ.cntrInc = -1;
        motorSettingsZ.drctVal = PIN_STATUS_HIGH;
    }

    return {dstPlanxy, vecCorexy, motorSettingsA, motorSettingsB, motorSettingsZ, lenPlanxy, microsTotal};
}

bool Coords::addBlock(block_planxy_____t dstPlanxy) {

    if (dstPlanxy.z == VALUE______RESET) {
        // TODO :: make sure there is a reset to the xy position in this coordinate
        // Machine::reset(blockPlanxy.x, blockPlanxy.y);
        // Machine::homedZ = false;

        // will cause up movement until z-switch is pressed
        // TODO :: limit z-movement to device z-range (?), actually the reset value will make it travel until the z-switch is pressed
        // with the very high z-value, normal acceleration calc would not work, therefore setting default velocities
        dstPlanxy.vi = MACHINE_HOME_V_Z;
        dstPlanxy.vo = MACHINE_HOME_V_Z;

        // TODO :: think about reference coord
        // TODO :: what needs to happen with the block-buffer (especially at machine init) in case of any reset, xy-reset or z-reset
    }

    // TODO :: check if this case still occurs and maybe handle in another way
    if (dstPlanxy.vi == 0 && dstPlanxy.vo == 0) {
        dstPlanxy.vi = MACHINE_HOME_VXY;
        dstPlanxy.vo = MACHINE_HOME_VXY;
    }

    // src coordinates (TODO :: make sure assuming a specific src-coordinate does not introduce some kind of drift as potential step error accumulates)
    block_planxy_____t srcPlanxyB = Coords::blockBufferDevice[(Coords::blockIndex - 1) % BLOCK_BUFFER_SIZE].dstPlanxy;  // get the previous block
    coord_planxy_____t srcPlanxyC = {srcPlanxyB.x, srcPlanxyB.y, srcPlanxyB.z};

    Coords::blockBufferDevice[Coords::blockIndex % BLOCK_BUFFER_SIZE] = toDeviceBlock(srcPlanxyC, dstPlanxy);
    Coords::blockIndex++;

    return true;
}

uint16_t Coords::getBuffSize() {
    return Coords::nextBlockIndex + BLOCK_BUFFER_SIZE - Coords::blockIndex;
}

coord_corexy_____t Coords::planxyToCorexy(coord_planxy_____t& coordPlanar) {
    return {(int32_t)round((coordPlanar.x - coordPlanar.y) * MOTOR___STEPS_MM), (int32_t)round((coordPlanar.x + coordPlanar.y) * MOTOR___STEPS_MM), (int32_t)round(coordPlanar.z * MOTOR_Z_STEPS_MM)};
}

coord_corexy_____t Coords::planxyToCorexy(block_planxy_____t& blockPlanar) {
    return {(int32_t)round((blockPlanar.x - blockPlanar.y) * MOTOR___STEPS_MM), (int32_t)round((blockPlanar.x + blockPlanar.y) * MOTOR___STEPS_MM), (int32_t)round(blockPlanar.z * MOTOR_Z_STEPS_MM)};
}

coord_planxy_____t Coords::corexyToPlanxy(coord_corexy_____t& coordCorexy) {
    return {(coordCorexy.a + coordCorexy.b) * 0.5 / MOTOR___STEPS_MM, (coordCorexy.b - coordCorexy.a) * 0.5 / MOTOR___STEPS_MM, coordCorexy.z * 1.0 / MOTOR_Z_STEPS_MM};
}

coord_corexy_____t Coords::toCorexyVector(coord_corexy_____t& srcCorexy, coord_corexy_____t& dstCorexy) {
    return {dstCorexy.a - srcCorexy.a, dstCorexy.b - srcCorexy.b, dstCorexy.z - srcCorexy.z};
}

coord_planxy_____t Coords::toPlanxyVector(coord_planxy_____t& srcPlanxy, block_planxy_____t& dstPlanxy) {
    return {dstPlanxy.x - srcPlanxy.x, dstPlanxy.y - srcPlanxy.y, dstPlanxy.z - srcPlanxy.z};
}

double Coords::toLength(coord_planxy_____t& coordPlanxy) {
    return sqrt(coordPlanxy.x * coordPlanxy.x + coordPlanxy.y * coordPlanxy.y + coordPlanxy.z * coordPlanxy.z);
}
