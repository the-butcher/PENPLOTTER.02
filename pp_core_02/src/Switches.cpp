#include "Switches.h"

// NOK :: 19, 35, 36, 39
// OK  :: 20, 21, 37, 38, 40, 47
// 20 is somehow associated with USB power
// Switch Switches::switchX('X', GPIO_NUM_38);
// Switch Switches::switchY('Y', GPIO_NUM_37);
// Switch Switches::switchZ('Z', GPIO_NUM_40);
Switch Switches::switchX('X', GPIO_NUM_47);
Switch Switches::switchY('Y', GPIO_NUM_38);
Switch Switches::switchZ('Z', GPIO_NUM_40);

bool Switches::begin() {
    return Switches::switchX.begin() && Switches::switchY.begin() && Switches::switchZ.begin();
}

bool Switches::isAnySwitchPressed() {
    return Switches::switchX.isPressed() || Switches::switchY.isPressed() || Switches::switchZ.isPressed();
}
