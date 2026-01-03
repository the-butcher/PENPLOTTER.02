#ifndef Motors_h
#define Motors_h

#include <Arduino.h>

#include "Coords.h"
#include "Define.h"
#include "Motor.h"

class Motors {
   private:
   public:
    static Motor motorA;
    static Motor motorB;
    static Motor motorZ;
    static Motor motorR;
    /**
     * begin the static Motors instance
     */
    static bool begin();
    static bool enable();
    /**
     * get current corexy coordinate from motor counters
     */
    static coord_corexy_____t getCurCorexy();
};

#endif