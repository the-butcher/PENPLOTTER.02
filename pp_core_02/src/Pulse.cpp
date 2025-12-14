#include "Pulse.h"

hw_timer_t* Pulse::timer;
uint16_t Pulse::divider = 8;
uint64_t Pulse::eventsPerSecond = MICROSECONDS_PER_SECOND * 80 / Pulse::divider;
uint64_t Pulse::microsLast = 0;
uint64_t Pulse::microsCurr = 0;
uint64_t Pulse::microsPulse = 0;
double Pulse::frequencyPulse = 0.0;
uint64_t Pulse::microsProcs = 0;
double Pulse::frequencyProcs = 0.0;
uint64_t Pulse::pulseCount = 0;

bool Pulse::begin() {

    // by padding 80 as divider, the internal value of 80 / divider will be 1
    Pulse::timer = timerBegin(0, Pulse::divider, true);  // 8 means 10000000 events per second (10MHz)
    timerAttachInterrupt(Pulse::timer, &Pulse::pulse, true);
    Pulse::setFrequency(1);
    timerAlarmEnable(Pulse::timer);

    return true;
}

void Pulse::pulse() {

    Pulse::microsLast = Pulse::microsCurr;
    Pulse::microsCurr = micros();
    Pulse::microsPulse = Pulse::microsCurr - Pulse::microsLast;
    Pulse::frequencyPulse = Pulse::microsPulse > 0 ? MICROSECONDS_PER_SECOND * 1.0 / Pulse::microsPulse : 0;

    Pulse::pulseCount++;
    Device::pulse();

    Pulse::microsProcs = micros() - Pulse::microsCurr;
    Pulse::frequencyProcs = Pulse::microsProcs > 0 ? MICROSECONDS_PER_SECOND * 1.0 / Pulse::microsProcs : 0;
}

void Pulse::yield() {
    Pulse::setFrequency(1);
}

void Pulse::setFrequency(double frequencyHz) {

    uint64_t alarmValue = round(Pulse::eventsPerSecond / frequencyHz);
    // Serial.print("frequencyHz: ");
    // Serial.println(frequencyHz);
    // Serial.print("alarmValue: ");
    // Serial.println(alarmValue);

    timerAlarmWrite(Pulse::timer, alarmValue, true);
}
