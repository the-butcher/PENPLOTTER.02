#ifndef Device_h
#define Device_h

#include <Arduino.h>

#include "Define.h"
#include "Driver.h"
#include "Motor.h"
#include "Motors.h"

class Device {
   private:
    static Motor* motorPrim;  // primary motor
    static Motor* motorSec1;  // secondary motor 1
    static Motor* motorSec2;  // secondary motor 2

   public:
    static uint64_t acceptMicros;
    static uint64_t acceptCount;
    static bool begin();

    /**
     * set the A and B counters to reflect the given planar coordinate
     */
    static void reset(int64_t x, int64_t y);

    // static bool accept(block_device_____t& dstDevice);  // accept a new destination coordinate and immediately start moving to, or drawing to that coordinate
    static bool accept(block_planxy_i64_t dstPlanxy);  // accept a new destination coordinate and immediately start moving to, or drawing to that coordinate

    static void pulse();

    /**
     * pause for a moment to make further decisions
     */
    static void yield();

    static bool homedX;
    static bool homedY;
    static bool homedZ;

    /**
     * length of the current segment in micrometers (µm, thousandths of millimeters)
     */
    static uint64_t lenP__um;

    /**
     * duration of the current segment in microseconds
     */
    static uint64_t durP__us;

    /**
     * entry-frequency of the current block
     */
    static uint64_t frqI_mHz;

    /**
     * exit-frequency of the current block
     */
    static uint64_t frqO_mHz;

    /**
     * frequency-acceleration of the current block doubled
     */
    static uint64_t frqA2;

    /**
     * entry-frequency of the current block squared
     */
    static uint64_t frqII;

    /**
     * primary counter
     */
    static uint32_t cPrim;

    /**
     * primary delta
     */
    static uint32_t dPrim;

    /**
     * secondary delta 1
     */
    static uint32_t dSec1;

    /**
     * secondary delta 2
     */
    static uint32_t dSec2;

    /**
     * error 1
     */
    static int32_t eSec1;

    /**
     * error 2
     */
    static int32_t eSec2;
};

#endif