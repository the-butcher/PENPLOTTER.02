#include "Motor.h"

void logSettingsMicro(uint8_t index, motor_microstep__t settingsMicro) {
    Serial.print(String(index));
    Serial.print(", x");
    Serial.print(String(settingsMicro.micrMlt));
    Serial.print(", [");
    Serial.print(String(settingsMicro.micrVa0));
    Serial.print(", ");
    Serial.print(String(settingsMicro.micrVa1));
    Serial.print(", ");
    Serial.print(String(settingsMicro.micrVa2));
    Serial.println("]");
}

Motor::Motor(char id, uint8_t stepsMm, gpio_num_t enabPin, gpio_num_t stepPin, gpio_num_t drctPin, motor_microstep__t settingsMicro) {
    this->id = id;
    this->stepsMm = stepsMm;
    this->enabPin = enabPin;
    this->stepPin = stepPin;
    this->drctPin = drctPin;
    this->settingsMicro[0] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};  // setting everything to HIGH corresponds to all micro pins being set to enabPin
    this->settingsMicro[1] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};
    this->settingsMicro[2] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};
    this->settingsMicro[3] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};
    this->micrPn0 = enabPin;
    this->micrPn1 = enabPin;
    this->micrPn2 = enabPin;
    this->cntrCur = 0;
    this->micrCur = 0;
}

Motor::Motor(char id, uint8_t stepsMm, gpio_num_t enabPin, gpio_num_t stepPin, gpio_num_t drctPin, gpio_num_t micrPn0, gpio_num_t micrPn1, gpio_num_t micrPn2, const motor_microstep__t settingsMicro0, const motor_microstep__t settingsMicro1, const motor_microstep__t settingsMicro2, const motor_microstep__t settingsMicro3) {
    this->id = id;
    this->stepsMm = stepsMm;
    this->enabPin = enabPin;
    this->stepPin = stepPin;
    this->drctPin = drctPin;
    this->settingsMicro[0] = {1, settingsMicro0.micrVa0, settingsMicro0.micrVa1, settingsMicro0.micrVa2};
    this->settingsMicro[1] = {(uint8_t)(settingsMicro1.micrMlt / settingsMicro0.micrMlt), settingsMicro1.micrVa0, settingsMicro1.micrVa1, settingsMicro1.micrVa2};
    this->settingsMicro[2] = {(uint8_t)(settingsMicro2.micrMlt / settingsMicro0.micrMlt), settingsMicro2.micrVa0, settingsMicro2.micrVa1, settingsMicro2.micrVa2};
    this->settingsMicro[3] = {(uint8_t)(settingsMicro3.micrMlt / settingsMicro0.micrMlt), settingsMicro3.micrVa0, settingsMicro3.micrVa1, settingsMicro3.micrVa2};
    this->micrPn0 = micrPn0;
    this->micrPn1 = micrPn1;
    this->micrPn2 = micrPn2;
    this->cntrCur = 0;
    this->micrCur = 0;
}

motor_microstep__t Motor::findMicrostepSettings(double baseFrequency) {
    // Serial.print(String(baseFrequency, 3));
    // Serial.print("Hz, ");
    if (baseFrequency * this->settingsMicro[3].micrMlt <= MAX_FREQUENCY) {  // for motors not having microstep settings this should be true
        // logSettingsMicro(3, this->settingsMicro[3]);
        return this->settingsMicro[3];
    } else if (baseFrequency * this->settingsMicro[2].micrMlt <= MAX_FREQUENCY) {  // for motors not having microstep settings this should be true
        // logSettingsMicro(2, this->settingsMicro[2]);
        return this->settingsMicro[2];
    } else if (baseFrequency * this->settingsMicro[1].micrMlt <= MAX_FREQUENCY) {  // for motors not having microstep settings this should be true
        // logSettingsMicro(1, this->settingsMicro[1]);
        return this->settingsMicro[1];
    } else {
        // logSettingsMicro(0, this->settingsMicro[0]);
        return this->settingsMicro[0];  // the coarsest settings, which can be run with the lowest frequency
    }
}

void Motor::applySettings(motor_settings___t setsCur) {
    this->setsCur = setsCur;
    digitalWrite(this->enabPin, PIN_STATUS_HIGH);
    digitalWrite(this->drctPin, this->setsCur.drctVal);
    digitalWrite(this->micrPn0, this->setsCur.settingsMicro.micrVa0);
    digitalWrite(this->micrPn1, this->setsCur.settingsMicro.micrVa1);
    digitalWrite(this->micrPn2, this->setsCur.settingsMicro.micrVa2);
}

bool Motor::begin() {

    // logSettingsMicro(0, this->settingsMicro[0]);
    // logSettingsMicro(1, this->settingsMicro[1]);
    // logSettingsMicro(2, this->settingsMicro[2]);
    // logSettingsMicro(3, this->settingsMicro[3]);

    pinMode(this->enabPin, OUTPUT);
    pinMode(this->stepPin, OUTPUT);
    pinMode(this->drctPin, OUTPUT);
    pinMode(this->micrPn0, OUTPUT);
    pinMode(this->micrPn1, OUTPUT);
    pinMode(this->micrPn2, OUTPUT);

    this->applySettings({PIN_STATUS__LOW, 1, this->settingsMicro[0]});  // apply  initial settings to have all pins in a valid state

    return true;
}

void Motor::pulse() {
    digitalWrite(this->stepPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->stepPin, LOW);
    this->micrCur++;
    if (this->micrCur >= this->setsCur.settingsMicro.micrMlt) {
        this->micrCur = 0;
        this->cntrCur += this->setsCur.cntrInc;
    }
}

void Motor::setCntrCur(int32_t cntrCur) {
    this->cntrCur = cntrCur;
    this->micrCur = 0;
}

int32_t Motor::getCntrCur() {
    return this->cntrCur;
}