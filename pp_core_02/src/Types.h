#ifndef Types_h
#define Types_h

#include <Arduino.h>

typedef struct {
    double x;          // x coord with respect to x home (mm)
    double y;          // y coord with respect to y home (mm)
    double z;          // z coord with respect to z home (mm)
} coord_planxy_____t;  // 12 bytes

typedef struct {
    double x;          // exit x coord with respect to x home (mm)
    double y;          // exit y coord with respect to y home (mm)
    double z;          // exit z coord with respect to z home (mm)
    double vi;         // entry speed (mm/s)
    double vo;         // exit speed (mm/s)
} block_planxy_____t;  // 20 bytes

typedef struct {
    int32_t a;         // left motor coord (microstep settings as of motor-A constants)
    int32_t b;         // right motor coord (microstep settings as of motor-B constants)
    int32_t z;         // pen motor coord (microstep settings as of motor-Z constants)
} coord_corexy_____t;  // 12 bytes

typedef enum {
    PIN_STATUS__LOW,
    PIN_STATUS_HIGH
} pin_status_______e;

// typedef enum {
//     MOTOR_ORDER_ABZ,
//     MOTOR_ORDER_BAZ,
//     MOTOR_ORDER_ZAB
// } motor_order______e;

typedef struct {
    uint8_t micrMlt;             // the microstep multiplier of this setting (4, 8, 16, 32)
    pin_status_______e micrVa0;  // status to be set on the microstep pin M0 to achieve the given microstep resolution
    pin_status_______e micrVa1;  // status to be set on the microstep pin M1 to achieve the given microstep resolution
    pin_status_______e micrVa2;  // status to be set on the microstep pin M2 to achieve the given microstep resolution
} motor_microstep__t;

typedef struct {
    pin_status_______e drctVal;        // status to be set on the direction pin
    int8_t cntrInc;                    // increment to be applied to the counter, either +1 or -1
    motor_microstep__t settingsMicro;  // microstep settings (resolution, pin values)
} motor_settings___t;

typedef struct {
    block_planxy_____t dstPlanxy;  // the destination coordinate
    coord_corexy_____t vecCorexy;  // the corexy vector for reference (microstepping not multiplied yet)
    motor_settings___t settingsA;  // motorSettings A (direction, microstepping, ...)
    motor_settings___t settingsB;  // motorSettings B (direction, microstepping, ...)
    motor_settings___t settingsZ;  // motorSettings Z (direction, microstepping, ...)
    double lenPlanxy;              // length to destination in mm
    uint64_t microsTotal;          // total duration in microseconds to complete this block
} block_device_____t;              // 96 bytes

// const t = sizeof(block_device_____t);

#endif