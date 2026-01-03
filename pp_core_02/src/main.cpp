#include <Arduino.h>

#include "Blesrv.h"
#include "Coords.h"
#include "Define.h"
#include "Device.h"
#include "Driver.h"
#include "Motors.h"
#include "Switches.h"

// uint64_t microsProcsMax = 0;

void driverBegin(void* pvParameters) {
    // Serial.print("pulseBegin: ");
    // Serial.println(xPortGetCoreID());
    Driver::begin();
    vTaskDelete(NULL);
}

// void _setup() {

//     neopixelWrite(RGB_BUILTIN, 1, 1, 0);  // yellow

//     Serial.begin(115200);
//     delay(5000);
//     Serial.print("PP: setup - 1, ESP.getFreeHeap(): ");
//     Serial.println(ESP.getFreeHeap());

//     Coords::begin();
//     Motors::begin();
//     Driver::begin();

//     block_planxy_f___t blockPlanxy_f = {297.0, 420.0, -8.0, 5.0, 20.0};
//     block_planxy_i64_t blockPlanxy_i = Coords::planxyToPlanxy(blockPlanxy_f);
//     Device::accept(blockPlanxy_i);

//     for (uint8_t p = 0; p < 1000; p++) {
//         Driver::pulse();
//     }

//     neopixelWrite(RGB_BUILTIN, 0, 0, 0);  // off
// }

void setup() {

    neopixelWrite(RGB_BUILTIN, 1, 1, 0);  // yellow

    Serial.begin(115200);
    delay(5000);
    Serial.print("PP: setup - 1, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());

    Switches::begin();  // initial regular RGB values, actually turns off RGB led

    Motors::begin();
    Coords::begin();  // adds machine homing coordinates
    Device::begin();  // currently does nothing

    pinMode(GPIO_NUM_44, OUTPUT);

    Blesrv::begin();
    Serial.println("PP: setup - waiting for bluetooth connection ...");
    uint64_t connectCounter = 0;
    while (!Blesrv::isConnected()) {
        if (connectCounter % 5 == 0) {
            // neopixelWrite(RGB_BUILTIN, 0, 0, 2);  // blue
            digitalWrite(GPIO_NUM_44, LOW);  // LOW is ON
            Blesrv::setLedStatus(BLUE_STATUS__ON);
            delay(200);
            // neopixelWrite(RGB_BUILTIN, 0, 0, 0);  // off
            Blesrv::setLedStatus(BLUE_STATUS_OFF);
            delay(200);
        } else {
            delay(400);
        }
        connectCounter++;
    }
    Serial.println("PP: setup - ... bluetooth connection established");
    Blesrv::setLedStatus(BLUE_STATUS__ON);
    // neopixelWrite(RGB_BUILTIN, 0, 0, 2);  // blue

    Motors::enable();

    // set initial buffer size and position
    Blesrv::writeBuffSize();
    Blesrv::writePosition();
    Serial.println("PP: setup - ... initial values written");

    // start machine pulse (the driving interval) on core 0
    xTaskCreatePinnedToCore(driverBegin, "driver-begin", 100000, NULL, 1, NULL, 0);

    Serial.print("PP: setup - 3, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());

    // block_planxy_f___t blockPlanxy_f = {100.0, -100.0, -8.0, 30.0, 5.0};
    // block_planxy_i64_t blockPlanxy_i = Coords::planxyToPlanxy(blockPlanxy_f);
    // Coords::addBlock(blockPlanxy_i);
    // for (uint8_t p = 0; p < 100; p++) {
    //     Driver::pulse();
    // }
}

void loop() {

    // if (Motors::motorA.setsCur.settingsMicro.microMlt == 8) {         // 32
    //     neopixelWrite(RGB_BUILTIN, 0, 0, 4);                          // blue
    // } else if (Motors::motorA.setsCur.settingsMicro.microMlt == 4) {  // 16
    //     neopixelWrite(RGB_BUILTIN, 0, 4, 0);                          // green
    // } else if (Motors::motorA.setsCur.settingsMicro.microMlt == 2) {  // 8
    //     neopixelWrite(RGB_BUILTIN, 2, 2, 0);                          // yellow
    // } else if (Motors::motorA.setsCur.settingsMicro.microMlt == 1) {  // 4
    //     neopixelWrite(RGB_BUILTIN, 4, 0, 0);                          // red
    // }

    // TODO :: different behaviour when BLE is not connected

    Serial.print("acceptMicros: ");
    Serial.println(Device::acceptCount > 0 ? Device::acceptMicros / Device::acceptCount : 0);

    Switches::updateNeopixel();
    // uint8_t rVal = Switches::limitX.isPressed() ? 3 : 0;
    // uint8_t gVal = Switches::limitY.isPressed() ? 3 : 0;
    // uint8_t bVal = Switches::limitZ.isPressed() ? 3 : 0;
    // neopixelWrite(RGB_BUILTIN, rVal, gVal, bVal);

    // char outputBuf[128];
    // sprintf(outputBuf, "frqI: %6.2f, lenP__um: %6.2f", Device::frqI, Device::lenP__um);
    // Serial.println(outputBuf);

    Blesrv::writePosition();
    for (uint8_t i = 0; i < 5; i++) {
        Blesrv::writeBuffSize();  // only writes when the current value is not equal to the last written value
        delay(100);
    }

    // delay(400);
}
