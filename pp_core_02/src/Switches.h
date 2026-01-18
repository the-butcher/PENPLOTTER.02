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
    static void readAll();
    static bool isAnyPressed();  // TODO :: determine need
    static void updateNeopixel();
};

#endif