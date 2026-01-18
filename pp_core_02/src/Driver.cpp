#include "Driver.h"

hw_timer_t* Driver::timer;
uint16_t Driver::divider = 8;
uint64_t Driver::eventsPer1000Second = ONE_SECOND_____us * 1000L * 80L / Driver::divider;  // 10000000
// uint64_t Driver::microsLast = 0;
// uint64_t Driver::microsCurr = 0;
uint64_t Driver::pulseCount = 0;
uint64_t Driver::frq___mHz = 0;

bool Driver::begin() {

#ifndef USE_SERIAL
    Driver::timer = timerBegin(0, Driver::divider, true);  // 8 means 10000000 events per second (10MHz)
    timerAttachInterrupt(Driver::timer, &Driver::pulse, true);
    Driver::setFrq___mHz(IDLE_FREQUENCY_mHz);
    timerAlarmEnable(Driver::timer);
#endif

    return true;
}

void Driver::pulse() {

    // Driver::microsLast = Driver::microsCurr;
    // Driver::microsCurr = micros();

    Driver::pulseCount++;
    Device::pulse();
}

void Driver::yield() {
    Driver::setFrq___mHz(IDLE_FREQUENCY_mHz);
}

void Driver::setFrq___mHz(uint64_t frq___mHz) {

    Driver::frq___mHz = frq___mHz;

    uint64_t alarmValue = Driver::eventsPer1000Second / Driver::frq___mHz;

#ifdef USE_SERIAL
    Serial.print("frq___mHz: ");
    Serial.println(frq___mHz);
    Serial.print("alarmValue: ");
    Serial.println(alarmValue);
#endif

#ifndef USE_SERIAL
    timerAlarmWrite(Driver::timer, alarmValue, true);
#endif
}
