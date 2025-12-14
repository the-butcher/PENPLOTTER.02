#include <Arduino.h>

#include "CoordService.h"
#include "Coords.h"
#include "Define.h"
#include "Device.h"
#include "Motors.h"
#include "Pulse.h"

void pulseBegin(void* pvParameters) {
    // Serial.print("pulseBegin: ");
    // Serial.println(xPortGetCoreID());
    Pulse::begin();
    vTaskDelete(NULL);
}

void setup() {

    Serial.begin(115200);
    delay(5000);
    Serial.println("PP: setup - 1");

    // CoordService::begin();
    // Serial.println("PP: setup - waiting for bluetooth connection ...");
    // while (!CoordService::isConnected()) {
    //     delay(1000);
    // }
    // set initial buffer size and position (TODO :: Machine and Motors need to be ready at this point)
    // CoordService::setBuffSize();
    // CoordService::setPosition();
    // Serial.println("PP: setup - 2");

    Motors::begin();
    Coords::begin();  // adds machine homing coordinates
    Device::begin();  // currently does nothing
    // Switches::begin();

    // make sure the pulse is running on core 0
    xTaskCreatePinnedToCore(pulseBegin, "pulse-begin", 100000, NULL, 1, NULL, 0);

    Serial.println("PP: setup - 2");
}

void loop() {

    Serial.print("frequencyPulse: ");
    Serial.print(Pulse::frequencyPulse);
    Serial.print(", microsProcs: ");
    Serial.print(Pulse::microsProcs);
    Serial.print(", micrMlt-b: ");
    Serial.println(Motors::motorB.setsCur.settingsMicro.micrMlt);
    // Serial.print(", ESP.getFreeHeap(): ");
    // Serial.println(ESP.getFreeHeap());

    // char coordBuffer[64];
    // sprintf(coordBuffer, "a: %6d b: %6d z: %6d", Motors::motorA.getCntrCur(), Motors::motorB.getCntrCur(), Motors::motorZ.getCntrCur());
    // Serial.println(coordBuffer);

    // Serial.print(", pulseCount (m): ");
    // Serial.println(pulseCount);

    delay(1000);
}
