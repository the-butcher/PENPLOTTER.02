#include <Arduino.h>

#include "Blesrv.h"
#include "Coords.h"
#include "Define.h"
#include "Device.h"
#include "Driver.h"
#include "Motors.h"

// uint64_t microsProcsMax = 0;

void driverBegin(void* pvParameters) {
    // Serial.print("pulseBegin: ");
    // Serial.println(xPortGetCoreID());
    Driver::begin();
    vTaskDelete(NULL);
}

void setup() {

    neopixelWrite(RGB_BUILTIN, 1, 1, 0);  // yellow

    Serial.begin(115200);
    delay(5000);
    Serial.print("PP: setup - 1, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());

    Blesrv::begin();
    Serial.println("PP: setup - waiting for bluetooth connection ...");
    uint64_t connectCounter = 0;
    while (!Blesrv::isConnected()) {
        if (connectCounter % 10 == 0) {
            neopixelWrite(RGB_BUILTIN, 0, 0, 1);  // blue
            delay(100);
            neopixelWrite(RGB_BUILTIN, 0, 0, 0);  // off
            delay(300);
        } else {
            delay(400);
        }
        connectCounter++;
    }
    Serial.println("PP: setup - ... bluetooth connection established");
    neopixelWrite(RGB_BUILTIN, 0, 0, 1);  // blue

    Motors::begin();
    Coords::begin();  // adds machine homing coordinates
    Device::begin();  // currently does nothing
    // Switches::begin();

    // set initial buffer size and position
    Blesrv::writeBuffSize();
    Blesrv::writePosition();
    Serial.println("PP: setup - ... initial values written");

    // start machine pulse (the driving interval) on core 0
    xTaskCreatePinnedToCore(driverBegin, "driver-begin", 100000, NULL, 1, NULL, 0);

    Serial.print("PP: setup - 3, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());
}

void loop() {

    // TODO :: different behaviour when BLE is not connected

    // Serial.print("microsPulse: ");
    // Serial.print(Driver::microsPulse);
    // if (Driver::microsProcs > microsProcsMax) {
    // microsProcsMax = Driver::microsProcs;
    Serial.print("acceptMicros: ");
    Serial.println(Device::acceptCount > 0 ? Device::acceptMicros / Device::acceptCount : 0);
    // Serial.print(", microsProcs: ");
    // Serial.print(microsProcsMax);
    // Serial.print(", frequencyProcs: ");
    // Serial.println(Driver::frequencyProcs);
    // }

    // Serial.print(", micrMlt-b: ");
    // Serial.println(Motors::motorB.setsCur.settingsMicro.micrMlt);

    // char coordBuffer[64];
    // sprintf(coordBuffer, "a: %6d b: %6d z: %6d", Motors::motorA.getCntrCur(), Motors::motorB.getCntrCur(), Motors::motorZ.getCntrCur());
    // Serial.println(coordBuffer);

    // Serial.print(", pulseCount (m): ");
    // Serial.println(pulseCount);
    for (uint8_t i = 0; i < 10; i++) {
        Blesrv::writeBuffSize();  // only writes when the current value is not equal to the last written value
        delay(100);
    }
}
