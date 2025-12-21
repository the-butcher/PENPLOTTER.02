#include "Driver.h"

hw_timer_t* Driver::timer;
uint16_t Driver::divider = 8;
uint64_t Driver::eventsPerSecond = ONE_SECOND_____us * 80 / Driver::divider;  // 10000000
uint64_t Driver::microsLast = 0;
uint64_t Driver::microsCurr = 0;
uint64_t Driver::microsPulse = 0;
double Driver::frequencyPulse = 0.0;
uint64_t Driver::microsProcs = 0;
double Driver::frequencyProcs = 0.0;
uint64_t Driver::pulseCount = 0;

bool Driver::begin() {

    Driver::timer = timerBegin(0, Driver::divider, true);  // 8 means 10000000 events per second (10MHz)
    timerAttachInterrupt(Driver::timer, &Driver::pulse, true);
    Driver::setFrequency(1);
    timerAlarmEnable(Driver::timer);

    return true;
}

void Driver::pulse() {

    Driver::microsLast = Driver::microsCurr;
    Driver::microsCurr = micros();
    Driver::microsPulse = Driver::microsCurr - Driver::microsLast;
    Driver::frequencyPulse = Driver::microsPulse > 0 ? ONE_SECOND_____us * 1.0 / Driver::microsPulse : 0;

    Driver::pulseCount++;
    Device::pulse();

    Driver::microsProcs = micros() - Driver::microsCurr;
    Driver::frequencyProcs = Driver::microsProcs > 0 ? ONE_SECOND_____us * 1.0 / Driver::microsProcs : 0;
}

void Driver::yield() {
    Driver::setFrequency(1);
}

void Driver::setFrequency(double frequencyHz) {

    // Serial.print("frequencyHz: ");
    // Serial.println(frequencyHz);

    uint64_t alarmValue = round(Driver::eventsPerSecond / frequencyHz);

    // Serial.print("alarmValue: ");
    // Serial.println(alarmValue);

    timerAlarmWrite(Driver::timer, alarmValue, true);
}
