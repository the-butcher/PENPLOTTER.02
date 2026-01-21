#include "Switch.h"

Switch::Switch(char id, uint8_t gpin) {
    this->id = id;
    this->gpin = gpin;
    this->ipin = digitalPinToInterrupt(gpin);
    this->pressed = false;
}

void Switch::read() {
    this->pressed = digitalRead(this->gpin);
}

bool Switch::begin() {
    pinMode(this->gpin, INPUT_PULLUP);  // this makes HIGH (=pressed) the default and the switch must actively pull it low
    return true;
}

bool Switch::isPressed() {
#ifdef USE_SWITCHES
    return this->pressed == HIGH;
#else
    return false;
#endif
}