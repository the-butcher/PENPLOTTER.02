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
    /**
     * begin the static Motors instance
     */
    static bool begin();
    /**
     * get current corexy coordinate from motor counters
     */
    static coord_corexy_____t getCurCorexy();
};

#endif