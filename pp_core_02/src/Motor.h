#ifndef Motor_h
#define Motor_h

#include <Arduino.h>

#include "Define.h"
#include "Types.h"

const motor_microstep__t MICROSTEP_T__04 = {4, PIN_STATUS__LOW, PIN_STATUS_HIGH, PIN_STATUS__LOW};
const motor_microstep__t MICROSTEP_T__08 = {8, PIN_STATUS_HIGH, PIN_STATUS_HIGH, PIN_STATUS__LOW};
const motor_microstep__t MICROSTEP_T__16 = {16, PIN_STATUS__LOW, PIN_STATUS__LOW, PIN_STATUS_HIGH};
const motor_microstep__t MICROSTEP_T__32 = {32, PIN_STATUS_HIGH, PIN_STATUS__LOW, PIN_STATUS_HIGH};
// const motor_microstep__t MICROSTEP_T_04_32[4] = {MICROSTEP_T__04, MICROSTEP_T__08, MICROSTEP_T__16, MICROSTEP_T__32};

class Motor {
   private:
    /**
     * steps per mm
     */
    uint8_t stepsMm;  // (steps-per-revolution * millistepping (half, quarter, ...)) / mm-belt-per-revolution, i.e. (400steps * 4quarterstep) / (20t * 2mm) = 40

    gpio_num_t enabPin;
    gpio_num_t stepPin;
    gpio_num_t drctPin;
    motor_microstep__t settingsMicro[4];
    gpio_num_t micrPn0;  // pin to apply the micrVa0 setting to
    gpio_num_t micrPn1;
    gpio_num_t micrPn2;
    int32_t cntrCur;  // current step count in base resolution
    uint8_t micrCur;  // current micro step count with respect to base resolution (if base resolution is 4 and micro resolution is 16, micro count must be in the range 0-16/4 -> 0-4)

   public:
    motor_settings___t setsCur;

    Motor(char id, uint8_t stepsMm, gpio_num_t enabPin, gpio_num_t stepPin, gpio_num_t drctPin, motor_microstep__t settingsMicro);
    Motor(char id, uint8_t stepsMm, gpio_num_t enabPin, gpio_num_t stepPin, gpio_num_t drctPin, gpio_num_t micrPn0, gpio_num_t micrPn1, gpio_num_t micrPn2, const motor_microstep__t settingsMicro0, const motor_microstep__t settingsMicro1, const motor_microstep__t settingsMicro2, const motor_microstep__t settingsMicro3);

    char id;

    /**
     * set everything needed to have a functional motor
     * - set direction pin to output
     * - set step pin to output
     */
    bool begin();

    void applySettings(motor_settings___t drctCur);

    /**
     * send a single motor pulse
     * - set the step pin to HIGH
     * - delay a minimum amount of time, i.e. one microsecond
     * - set the step pin to LOW
     * - increment the step counter with the appropriate value
     */
    void pulse();

    void setCntrCur(int32_t cntrCur);
    int32_t getCntrCur();

    /**
     * find the finest possible motor settings for the given base frequency
     */
    motor_microstep__t findMicrostepSettings(double baseFrequency);
};

#endif