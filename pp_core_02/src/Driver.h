#ifndef Driver_h
#define Driver_h

#include <Arduino.h>

#include "Define.h"
#include "Device.h"

class Driver {
   private:
    static hw_timer_t* timer;
    static uint16_t divider;
    static uint64_t eventsPer1000Second;
    static uint64_t microsLast;
    static uint64_t microsCurr;

   public:
    static uint64_t pulseCount;

    static double frequencyPulse;
    static uint64_t microsPulse;
    /**
     * the theoretical max-frequency, given the execution time
     */
    static double frequencyProcs;
    static uint64_t microsProcs;

    static bool begin();
    static void pulse();
    static void yield();

    /**
     * adjust the pulse timer to run at a the given frequency in hz (iterations / second)
     */
    static void setFrq_mHz(uint64_t frq_mHz);
};

#endif