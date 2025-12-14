#ifndef Device_h
#define Device_h

#include <Arduino.h>

#include "Define.h"
#include "Motor.h"
#include "Motors.h"
#include "Pulse.h"
#include "Types.h"

class Device {
   private:
    static Motor* motorPrim;  // primary motor
    static Motor* motorSec1;  // secondary motor 1
    static Motor* motorSec2;  // secondary motor 2

   public:
    static bool begin();

    /**
     * set the A and B counters to reflect the given planar coordinate
     */
    static void reset(double x, double y);

    static bool accept(block_device_____t& dstDevice);  // accept a new destination coordinate and immediately start moving to, or drawing to that coordinate

    static void pulse();

    /**
     * pause for a moment to make further decisions
     */
    static void yield();

    static bool homedX;
    static bool homedY;
    static bool homedZ;

    /**
     * entry-frequency of the current block
     */
    static double frqI;

    /**
     * exit-frequency of the current block
     */
    static double frqO;

    /**
     * frequency-acceleration of the current block doubled
     */
    static double frqA2;

    /**
     * entry-frequency of the current block squared
     */
    static double frqII;

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