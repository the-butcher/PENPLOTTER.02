#include "Motors.h"

Motor Motors::motorA('A', GPIO_NUM_9, GPIO_NUM_41, GPIO_NUM_42, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, MICROSTEP_T__04, MICROSTEP_T__08, MICROSTEP_T__16, MICROSTEP_T__32);
Motor Motors::motorB('B', GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_8, MICROSTEP_T__04, MICROSTEP_T__08, MICROSTEP_T__16, MICROSTEP_T__32);
Motor Motors::motorZ('Z', GPIO_NUM_7, GPIO_NUM_15, GPIO_NUM_16, MICROSTEP_T__16);
Motor Motors::motorR('R', GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, MICROSTEP_T__08);

bool Motors::begin() {
    return Motors::motorA.begin() && Motors::motorB.begin() && Motors::motorZ.begin() && Motors::motorR.begin();
}

bool Motors::enable() {
    return Motors::motorA.enable() && Motors::motorB.enable() && Motors::motorZ.enable() && Motors::motorR.enable();
}

coord_corexy_____t Motors::getCurCorexy() {
    return {Motors::motorA.getCntrCur(), Motors::motorB.getCntrCur(), Motors::motorZ.getCntrCur()};
}
