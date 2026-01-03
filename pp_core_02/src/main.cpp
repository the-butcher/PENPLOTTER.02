#include <Arduino.h>

#include "Blesrv.h"
#include "Coords.h"
#include "Define.h"
#include "Device.h"
#include "Driver.h"
#include "Motors.h"
#include "Switches.h"

uint64_t loopCounter = 0;

void driverBegin(void* pvParameters) {
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
            digitalWrite(GPIO_NUM_44, LOW);  // LOW is ON
            Blesrv::setLedStatus(BLUE_STATUS__ON);
            delay(200);
            Blesrv::setLedStatus(BLUE_STATUS_OFF);
            delay(200);
        } else {
            delay(400);
        }
        connectCounter++;
    }
    Serial.println("PP: setup - ... bluetooth connection established");
    Blesrv::setLedStatus(BLUE_STATUS__ON);

    Motors::enable();

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

    // Serial.print("acceptMicros: ");
    // Serial.println(Device::acceptCount > 0 ? Device::acceptMicros / Device::acceptCount : 0);

    Switches::updateNeopixel();

    if (loopCounter % 5 == 0) {
        Blesrv::writePosition();
    }
    Blesrv::writeBuffSize();  // only writes when the current value is not equal to the last written value
    delay(100);

    loopCounter++;
}
