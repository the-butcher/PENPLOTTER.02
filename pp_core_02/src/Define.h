// #define USE_SERIAL

#define LIMIT_X_STOP_PIN 9
#define LIMIT_Y_STOP_PIN 10
#define LIMIT_Z_STOP_PIN 11

#define MACHINE_DIM____X 297
#define MACHINE_DIM____Y 420
#define MACHINE_DIM____Z -8
#define MACHINE_HOME_VXY 20
#define MACHINE_HOME_V_Z 5

#define VALUE______RESET 9999.0

#define COMMAND_SERVICE___UUID "791320d5-7f0a-4b58-89f6-cc2031479da4"

#define COMMAND_BUFF_SIZE_UUID "067c3c93-eb63-4905-b292-478642f8ae99"
#define DESCRIB_BUFF_SIZE_UUID "9dc82658-4e03-403d-95ba-79bfe97576b4"

#define COMMAND_BUFF_VALS_UUID "d3116fb9-adc1-4fc4-9cb4-ceb48925fa1b"
#define DESCRIB_BUFF_VALS_UUID "a50be0c4-572c-4718-a9c8-b37d4f7bb2d8"

#define COMMAND_POSITION__UUID "b7e24055-35c2-418e-be2e-b690a11cf3fa"
#define DESCRIB_POSITION__UUID "775afbfa-5d1f-4225-be01-3b004f5981f1"

#define COMMAND_BUFF_VALS_SIZE 24
#define BTLE_DEVICE_NAME "PEN_PLOTTER_02"

#define MAX_FREQUENCY 6000.0

#ifndef Define_h
#define Define_h

#include <Arduino.h>

const int64_t ONE_ROTATION___um = 40000;
const int64_t ONE_ROTATION_Z_um = 60000;
const int64_t ONE_SECOND_____us = 1000000;
const int64_t TWO_SECONDS____us = 2000000;

// /**
//  * internal representation of a planar coordinate
//  * double used to avoid float problems in interrupts
//  * https://forum.arduino.cc/t/how-to-do-simple-float-calculations-in-a-interrupt-handler-on-nodemcu-esp32/915384
//  */
// typedef struct {
//     double x;          // x coord with respect to x home (mm)
//     double y;          // y coord with respect to y home (mm)
//     double z;          // z coord with respect to z home (mm)
// } coord_planxy_d___t;  // 24 bytes

typedef struct {
    int64_t x;         // x coord with respect to x home (mm)
    int64_t y;         // y coord with respect to y home (mm)
    int64_t z;         // z coord with respect to z home (mm)
} coord_planxy_i64_t;  // 24 bytes

/**
 * communication representation of a planar coordinate
 * float used to save space in bluetooth communication
 */
typedef struct {
    float x;           // x coord with respect to x home (mm)
    float y;           // y coord with respect to y home (mm)
    float z;           // z coord with respect to z home (mm)
} coord_planxy_f___t;  // 12 bytes

// /**
//  * internal representation of a planar distance with entry and exit speeds
//  * double used to avoid float problems in interrupts
//  * https://forum.arduino.cc/t/how-to-do-simple-float-calculations-in-a-interrupt-handler-on-nodemcu-esp32/915384
//  */
// typedef struct {
//     double x;          // exit x coord with respect to x home (mm)
//     double y;          // exit y coord with respect to y home (mm)
//     double z;          // exit z coord with respect to z home (mm)
//     double vi;         // entry speed (mm/s)
//     double vo;         // exit speed (mm/s)
// } block_planxy_d___t;  // 40 bytes

typedef struct {
    int64_t x;         // exit x coord with respect to x home (mm)
    int64_t y;         // exit y coord with respect to y home (mm)
    int64_t z;         // exit z coord with respect to z home (mm)
    int64_t vi;        // entry speed (mm/s)
    int64_t vo;        // exit speed (mm/s)
} block_planxy_i64_t;  // 40 bytes

/**
 * internal representation of a planar distance with entry and exit speeds
 * float used to save space in bluetooth communication
 */
typedef struct {
    float x;           // exit x coord with respect to x home (mm)
    float y;           // exit y coord with respect to y home (mm)
    float z;           // exit z coord with respect to z home (mm)
    float vi;          // entry speed (mm/s)
    float vo;          // exit speed (mm/s)
} block_planxy_f___t;  // 20 bytes

typedef struct {
    int64_t a;         // left motor coord (microstep settings as of motor-A constants)
    int64_t b;         // right motor coord (microstep settings as of motor-B constants)
    int64_t z;         // pen motor coord (microstep settings as of motor-Z constants)
} coord_corexy_____t;  // 12 bytes

typedef enum {
    PIN_STATUS__LOW,
    PIN_STATUS_HIGH
} pin_status_______e;

typedef struct {
    uint8_t microMlt;              // the microstep multiplier of this setting (4, 8, 16, 32)
    pin_status_______e microVal0;  // status to be set on the microstep pin M0 to achieve the given microstep resolution
    pin_status_______e microVal1;  // status to be set on the microstep pin M1 to achieve the given microstep resolution
    pin_status_______e microVal2;  // status to be set on the microstep pin M2 to achieve the given microstep resolution
} motor_microstep__t;

typedef struct {
    pin_status_______e directVal;      // status to be set on the direction pin
    int8_t counterIncrement;           // increment to be applied to the counter, either +1 or -1
    motor_microstep__t settingsMicro;  // microstep settings (resolution, pin values)
} motor_settings___t;                  // 24 bytes

// typedef struct {
//     block_planxy_d___t dstPlanxy;  // the destination coordinate
//     coord_corexy_____t vecCorexy;  // the corexy vector for reference (microstepping not multiplied yet)
//     motor_settings___t settingsA;  // motorSettings A (direction, microstepping, ...)
//     motor_settings___t settingsB;  // motorSettings B (direction, microstepping, ...)
//     motor_settings___t settingsZ;  // motorSettings Z (direction, microstepping, ...)
//     double lengthPlanxy;           // length to destination in mm
//     uint64_t microsTotal;          // total duration in microseconds to complete this block
// } block_device_____t;              // 144 bytes

#endif