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

void _setup() {

    neopixelWrite(RGB_BUILTIN, 1, 1, 0);  // yellow

    Serial.begin(115200);
    delay(5000);
    Serial.print("PP: setup - 1, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());

    Coords::begin();
    Motors::begin();
    Driver::begin();

    block_planxy_f___t blockPlanxy_f = {297.0, 420.0, -8.0, 5.0, 20.0};
    block_planxy_i64_t blockPlanxy_i = Coords::planxyToPlanxy(blockPlanxy_f);
    Device::accept(blockPlanxy_i);

    for (uint8_t p = 0; p < 1000; p++) {
        Driver::pulse();
    }

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);  // off
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

    Serial.print("acceptMicros: ");
    Serial.println(Device::acceptCount > 0 ? Device::acceptMicros / Device::acceptCount : 0);

    // char outputBuf[128];
    // sprintf(outputBuf, "frqI: %6.2f, lenP__um: %6.2f", Device::frqI, Device::lenP__um);
    // Serial.println(outputBuf);

    for (uint8_t i = 0; i < 10; i++) {
        Blesrv::writeBuffSize();  // only writes when the current value is not equal to the last written value
        delay(100);
    }
    Blesrv::writePosition();

    delay(1000);
}
