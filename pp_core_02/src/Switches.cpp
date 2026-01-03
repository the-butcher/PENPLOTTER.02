#include "Switches.h"

// NOK :: 19, 35, 36, 39
// OK  :: (20), 21, 37, 38, 40, 47
// 20 is somehow associated with USB power as USB will disconnect on pin level change
Switch Switches::limitX('X', GPIO_NUM_47);
Switch Switches::limitY('Y', GPIO_NUM_38);
Switch Switches::limitZ('Z', GPIO_NUM_40);

bool Switches::begin() {
    bool success = Switches::limitX.begin() && Switches::limitY.begin() && Switches::limitZ.begin();
    attachInterrupt(Switches::limitX.ipin, Switches::handleChangeX, CHANGE);
    attachInterrupt(Switches::limitY.ipin, Switches::handleChangeY, CHANGE);
    attachInterrupt(Switches::limitZ.ipin, Switches::handleChangeZ, CHANGE);
    Switches::updateNeopixel();
    return success;
}

void Switches::updateNeopixel() {
    uint8_t rVal = Switches::limitX.isPressed() ? 10 : 0;
    uint8_t gVal = Switches::limitY.isPressed() ? 10 : 0;
    uint8_t bVal = Switches::limitZ.isPressed() ? 10 : 0;
    neopixelWrite(RGB_BUILTIN, rVal, gVal, bVal);
}

void Switches::handleChangeX() {
    Switches::limitX.handleChange();
}

void Switches::handleChangeY() {
    Switches::limitY.handleChange();
}

void Switches::handleChangeZ() {
    Switches::limitZ.handleChange();
}

bool Switches::isAnyLimitPressed() {
    return Switches::limitX.isPressed() || Switches::limitY.isPressed() || Switches::limitZ.isPressed();
}
