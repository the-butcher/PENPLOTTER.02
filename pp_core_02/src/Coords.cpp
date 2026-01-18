#include "Coords.h"

// block_device_____t Coords::blockBufferDevice[BLOCK_BUFFER_SIZE];
block_planxy_i64_t Coords::blockBufferPlanxy[BLOCK_BUFFER_SIZE];

uint32_t Coords::nextBlockIndex = 0;
uint32_t Coords::blockIndex = 0;

/**
 * r-adapted
 */
bool Coords::begin() {

    block_planxy_f___t homeCoordinateZ = {0.0, 0.0, VALUE______RESET, 0.0, DEVICE___V__Z_mms, DEVICE___V__Z_mms};
    Coords::addBlock(Coords::planxyToPlanxy(homeCoordinateZ));  // move until z-swtich touched, z-homing the machine and accepting the z-home coordinate as 0.0

    block_planxy_f___t homeCoordinateX = {-VALUE______RESET, 0.0, 0.0, 0.0, DEVICE___V_XY_mms, DEVICE___V_XY_mms};  // move until x-switch touched, x-homing the machine and accepting the limit x-coordinate as -10.0
    Coords::addBlock(Coords::planxyToPlanxy(homeCoordinateX));

    homeCoordinateX = {0.0, 0.0, 0.0, 0.0, DEVICE___V_XY_mms, DEVICE___V_XY_mms};  // backup x to 0.0
    Coords::addBlock(Coords::planxyToPlanxy(homeCoordinateX));

    block_planxy_f___t homeCoordinateY = {0.0, -VALUE______RESET, 0.0, 0.0, DEVICE___V_XY_mms, DEVICE___V_XY_mms};  // move until y-swtich touched, y-homing the machine and accepting the limit y-coordinate as -20.0
    Coords::addBlock(Coords::planxyToPlanxy(homeCoordinateY));

    homeCoordinateY = {0.0, 0.0, 0.0, 0.0, DEVICE___V_XY_mms, DEVICE___V_XY_mms};  // backup y to 0.0, the machine will now be at x0.0, y0.0 with a lifted pen at z0.0
    Coords::addBlock(Coords::planxyToPlanxy(homeCoordinateY));

    return true;
}

bool Coords::hasBlock() {
    return Coords::nextBlockIndex < Coords::blockIndex;
}

block_planxy_i64_t Coords::popBlock() {
    return Coords::blockBufferPlanxy[Coords::nextBlockIndex++ % BLOCK_BUFFER_SIZE];
}

bool Coords::addBlock(block_planxy_i64_t blockPlanxy) {
    Coords::blockBufferPlanxy[Coords::blockIndex % BLOCK_BUFFER_SIZE] = blockPlanxy;
    Coords::blockIndex++;
    return true;
}

uint16_t Coords::getBuffSize() {
    return Coords::nextBlockIndex + BLOCK_BUFFER_SIZE - Coords::blockIndex;
}

/**
 * r-adapted
 */
coord_corexy_____t Coords::planxyToCorexy(coord_planxy_i64_t& coordPlanxy) {
    return {(int32_t)((coordPlanxy.x - coordPlanxy.y) * ONE_ROTATION___um / 1000000L), (int32_t)((coordPlanxy.x + coordPlanxy.y) * ONE_ROTATION___um / 1000000L), (int32_t)(coordPlanxy.z * ONE_ROTATION_Z_um / 1000000L), (int32_t)(coordPlanxy.r * ONE_ROTATION_R_um / 1000000L)};
}

/**
 * r-adapted
 */
coord_corexy_____t Coords::planxyToCorexy(block_planxy_i64_t& blockPlanxy) {
    return {(int32_t)((blockPlanxy.x - blockPlanxy.y) * ONE_ROTATION___um / 1000000L), (int32_t)((blockPlanxy.x + blockPlanxy.y) * ONE_ROTATION___um / 1000000L), (int32_t)(blockPlanxy.z * ONE_ROTATION_Z_um / 1000000L), (int32_t)(blockPlanxy.r * ONE_ROTATION_R_um / 1000000L)};
}

/**
 * r-adapted
 */
coord_planxy_i64_t Coords::corexyToPlanxy(coord_corexy_____t& coordCorexy) {
    return {(coordCorexy.a + coordCorexy.b) * 500000L / ONE_ROTATION___um, (coordCorexy.b - coordCorexy.a) * 500000L / ONE_ROTATION___um, coordCorexy.z * 1000000L / ONE_ROTATION_Z_um, coordCorexy.r * 1000000L / ONE_ROTATION_R_um};
}

/**
 * r-adapted
 */
coord_corexy_____t Coords::toCorexyVector(coord_corexy_____t& srcCorexy, coord_corexy_____t& dstCorexy) {
    return {dstCorexy.a - srcCorexy.a, dstCorexy.b - srcCorexy.b, dstCorexy.z - srcCorexy.z, dstCorexy.r - srcCorexy.r};
}

/**
 * r-adapted
 */
coord_planxy_i64_t Coords::toPlanxyVector(coord_planxy_i64_t& srcPlanxy, block_planxy_i64_t& dstPlanxy) {
    return {dstPlanxy.x - srcPlanxy.x, dstPlanxy.y - srcPlanxy.y, dstPlanxy.z - srcPlanxy.z, dstPlanxy.r - srcPlanxy.r};
}

/**
 * r-adapted
 */
uint64_t Coords::toLength(coord_planxy_i64_t& coordPlanxy) {
    return (uint64_t)sqrt(coordPlanxy.x * coordPlanxy.x + coordPlanxy.y * coordPlanxy.y + coordPlanxy.z * coordPlanxy.z + coordPlanxy.r * coordPlanxy.r);
}

/**
 * r-adapted
 */
block_planxy_i64_t Coords::planxyToPlanxy(block_planxy_f___t& blockPlanxy) {
    return {(int64_t)(blockPlanxy.x * 1000L), (int64_t)(blockPlanxy.y * 1000L), (int64_t)(blockPlanxy.z * 1000L), (int64_t)(blockPlanxy.r * 1000L), (int64_t)(blockPlanxy.vi * 1000L), (int64_t)(blockPlanxy.vo * 1000L)};
}

/**
 * r-adapted
 */
coord_planxy_f___t Coords::planxyToPlanxy(coord_planxy_i64_t& blockPlanxy) {
    return {blockPlanxy.x / 1000.0F, blockPlanxy.y / 1000.0F, blockPlanxy.z / 1000.0F, blockPlanxy.r / 1000.0F};
}