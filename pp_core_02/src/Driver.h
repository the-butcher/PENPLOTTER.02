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

   public:
    static uint64_t frq___mHz;

    static bool begin();
    static void pulse();
    static void yield();

    /**
     * adjust the pulse timer to run at a the given frequency in hz (iterations / second)
     */
    static void setFrq___mHz(uint64_t frq___mHz);
};

#endif