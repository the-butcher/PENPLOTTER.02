#include "Switch.h"

Switch::Switch(char id, uint8_t gpin) {
    this->id = id;
    this->gpin = gpin;
    this->ipin = digitalPinToInterrupt(gpin);
    this->pressed = false;
}

void Switch::handleChange() {
    this->pressed = digitalRead(this->gpin);
}

bool Switch::begin() {
    pinMode(this->gpin, INPUT_PULLUP);
    return true;
}

bool Switch::isPressed() {
    return this->pressed == HIGH;
}