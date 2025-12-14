#ifndef Coords_h
#define Coords_h

#include <Arduino.h>

#include "Coords.h"
#include "Define.h"
#include "Motors.h"
#include "Types.h"

const uint16_t BLOCK_BUFFER_SIZE = 512;

class Coords {
   private:
    static block_device_____t blockBufferDevice[BLOCK_BUFFER_SIZE];

   public:
    static bool begin();
    static block_device_____t popBlock();
    static bool hasBlock();
    static bool addBlock(block_planxy_____t blockPlanxy);
    /**
     * get the size of available buffer space
     */
    static uint16_t getBuffSize();

    static uint32_t nextBlockIndex;  // the index at which the next coordinate would be read
    static uint32_t blockIndex;      // the index at which the maximum buffered coordinate is (must never be large enough to overwrite coordinates not having been handled yet)

    static block_device_____t toDeviceBlock(coord_planxy_____t& srcPlanxy, block_planxy_____t& dstPlanxy);

    /**
     * implemented according to https://corexy.com/theory.html
     * NOTE :: a positive A-value relates to a counterclockwise rotation of the A-motor, a positive B-value relates to a counterclockwise rotation of the B-motor
     */
    static coord_corexy_____t planxyToCorexy(coord_planxy_____t& coordPlanxy);
    static coord_corexy_____t planxyToCorexy(block_planxy_____t& blockPlanxy);
    static coord_planxy_____t corexyToPlanxy(coord_corexy_____t& coordCorexy);

    static coord_corexy_____t toCorexyVector(coord_corexy_____t& srcCorexy, coord_corexy_____t& dstCorexy);
    static coord_planxy_____t toPlanxyVector(coord_planxy_____t& srcPlanxy, block_planxy_____t& dstPlanxy);

    static double toLength(coord_planxy_____t& coordPlanxy);
};

#endif