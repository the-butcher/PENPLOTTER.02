#include "Motor.h"

void logSettingsMicro(uint8_t index, motor_microstep__t settingsMicro) {
    Serial.print(String(index));
    Serial.print(", x");
    Serial.print(String(settingsMicro.microMlt));
    Serial.print(", [");
    Serial.print(String(settingsMicro.microVal0));
    Serial.print(", ");
    Serial.print(String(settingsMicro.microVal1));
    Serial.print(", ");
    Serial.print(String(settingsMicro.microVal2));
    Serial.println("]");
}

Motor::Motor(char id, gpio_num_t enablePin, gpio_num_t stepPin, gpio_num_t directPin, motor_microstep__t settingsMicro) {
    this->id = id;
    this->supportsMicro = false;
    this->enablePin = enablePin;
    this->stepPin = stepPin;
    this->directPin = directPin;
    this->settingsMicro[0] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};  // setting everything to HIGH corresponds to all micro pins being set to enablePin
    this->settingsMicro[1] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};
    this->settingsMicro[2] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};
    this->settingsMicro[3] = {1, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS_HIGH};
    this->maxBaseFrq_mHz[0] = MAX_FREQUENCY_mHz / this->settingsMicro[0].microMlt;
    this->maxBaseFrq_mHz[1] = MAX_FREQUENCY_mHz / this->settingsMicro[1].microMlt;
    this->maxBaseFrq_mHz[2] = MAX_FREQUENCY_mHz / this->settingsMicro[2].microMlt;
    this->maxBaseFrq_mHz[3] = MAX_FREQUENCY_mHz / this->settingsMicro[3].microMlt;
    this->microPin0 = enablePin;
    this->microPin1 = enablePin;
    this->microPin2 = enablePin;
    this->cntrCur = 0;
    this->micrCur = 0;
}

Motor::Motor(char id, gpio_num_t enablePin, gpio_num_t stepPin, gpio_num_t directPin, gpio_num_t microPin0, gpio_num_t microPin1, gpio_num_t microPin2, const motor_microstep__t settingsMicro0, const motor_microstep__t settingsMicro1, const motor_microstep__t settingsMicro2, const motor_microstep__t settingsMicro3) {
    this->id = id;
    this->supportsMicro = true;
    this->enablePin = enablePin;
    this->stepPin = stepPin;
    this->directPin = directPin;
    this->settingsMicro[0] = {1, settingsMicro0.microVal0, settingsMicro0.microVal1, settingsMicro0.microVal2};
    this->settingsMicro[1] = {(uint8_t)(settingsMicro1.microMlt / settingsMicro0.microMlt), settingsMicro1.microVal0, settingsMicro1.microVal1, settingsMicro1.microVal2};
    this->settingsMicro[2] = {(uint8_t)(settingsMicro2.microMlt / settingsMicro0.microMlt), settingsMicro2.microVal0, settingsMicro2.microVal1, settingsMicro2.microVal2};
    this->settingsMicro[3] = {(uint8_t)(settingsMicro3.microMlt / settingsMicro0.microMlt), settingsMicro3.microVal0, settingsMicro3.microVal1, settingsMicro3.microVal2};
    this->maxBaseFrq_mHz[0] = MAX_FREQUENCY_mHz / this->settingsMicro[0].microMlt;
    this->maxBaseFrq_mHz[1] = MAX_FREQUENCY_mHz / this->settingsMicro[1].microMlt;
    this->maxBaseFrq_mHz[2] = MAX_FREQUENCY_mHz / this->settingsMicro[2].microMlt;
    this->maxBaseFrq_mHz[3] = MAX_FREQUENCY_mHz / this->settingsMicro[3].microMlt;
    this->microPin0 = microPin0;
    this->microPin1 = microPin1;
    this->microPin2 = microPin2;
    this->cntrCur = 0;
    this->micrCur = 0;
}

motor_microstep__t Motor::findMicrostepSettings(uint64_t frq___mHz) {
    // Serial.print(String(baseFrequency, 3));
    // Serial.print("Hz, ");
    if (frq___mHz <= this->maxBaseFrq_mHz[3]) {  // baseFrequency * this->settingsMicro[3].microMlt <= MAX_FREQUENCY // for motors not having microstep settings this should be true
        // logSettingsMicro(3, this->settingsMicro[3]);
        return this->settingsMicro[3];
    } else if (frq___mHz <= this->maxBaseFrq_mHz[2]) {  // for motors not having microstep settings this should be true
        // logSettingsMicro(2, this->settingsMicro[2]);
        return this->settingsMicro[2];
    } else if (frq___mHz <= this->maxBaseFrq_mHz[1]) {  // for motors not having microstep settings this should be true
        // logSettingsMicro(1, this->settingsMicro[1]);
        return this->settingsMicro[1];
    } else {
        // logSettingsMicro(0, this->settingsMicro[0]);
        return this->settingsMicro[0];  // the coarsest settings, which can be run with the lowest frequency
    }
}

void Motor::applySettings(motor_settings___t setsCur) {
    this->setsCur = setsCur;
    // digitalWrite(this->enablePin, PIN_STATUS_HIGH);
    digitalWrite(this->directPin, this->setsCur.directVal);
    if (this->supportsMicro) {
        digitalWrite(this->microPin0, this->setsCur.settingsMicro.microVal0);
        digitalWrite(this->microPin1, this->setsCur.settingsMicro.microVal1);
        digitalWrite(this->microPin2, this->setsCur.settingsMicro.microVal2);
    }
}

bool Motor::begin() {

    // logSettingsMicro(0, this->settingsMicro[0]);
    // logSettingsMicro(1, this->settingsMicro[1]);
    // logSettingsMicro(2, this->settingsMicro[2]);
    // logSettingsMicro(3, this->settingsMicro[3]);

    pinMode(this->enablePin, OUTPUT);
    pinMode(this->stepPin, OUTPUT);
    pinMode(this->directPin, OUTPUT);
    pinMode(this->microPin0, OUTPUT);
    pinMode(this->microPin1, OUTPUT);
    pinMode(this->microPin2, OUTPUT);

    // enable the motor (for motors not having dynamic microswitching, this pulls the micropins high so any jumpers set will be in effect)
    digitalWrite(this->enablePin, PIN_STATUS_HIGH);

    this->applySettings({PIN_STATUS__LOW, 1, this->settingsMicro[0]});  // apply  initial settings to have all pins in a valid state

    return true;
}

void Motor::pulse() {
    digitalWrite(this->stepPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->stepPin, LOW);
    this->micrCur++;
    if (this->micrCur >= this->setsCur.settingsMicro.microMlt) {
        this->micrCur = 0;
        this->cntrCur += this->setsCur.counterIncrement;
    }
}

void Motor::setCntrCur(int32_t cntrCur) {
    this->cntrCur = cntrCur;
    this->micrCur = 0;
}

int32_t Motor::getCntrCur() {
    return this->cntrCur;
}