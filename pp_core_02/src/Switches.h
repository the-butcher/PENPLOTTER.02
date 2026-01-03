#ifndef Switches_h
#define Switches_h

#include <Arduino.h>
#include <Define.h>

#include "Switch.h"

class Switches {
   private:
   public:
    static Switch limitX;
    static Switch limitY;
    static Switch limitZ;
    static bool begin();
    static void handleChangeX();
    static void handleChangeY();
    static void handleChangeZ();
    static bool isAnyLimitPressed();
    static void updateNeopixel();
};

#endif