#include <Arduino.h>

#include "Blesrv.h"
#include "Coords.h"
#include "Define.h"
#include "Device.h"
#ifdef USE_DISPLAY
#include "Display.h"
#endif
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

#ifdef USE_DISPLAY
    Serial.println("PP: setup - 1, Display::powerup()");
    Display::powerup();
#endif

    Serial.begin(115200);
    delay(5000);
    Serial.print("PP: setup - 1, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());

#ifdef USE_DISPLAY
    Display::addMessage("begin motors");
#endif

    Motors::begin();  // TODO :: do this motor by motor with delays

#ifdef USE_DISPLAY
    Display::addMessage("begin coords");
#endif

    Coords::begin();  // adds machine homing coordinates

#ifdef USE_DISPLAY
    Display::addMessage("begin device");
#endif

    Device::begin();  // currently does nothing

#ifdef USE_DISPLAY
    Display::addMessage("begin bluetooth");
#endif

    Blesrv::begin();
    Serial.println("PP: setup - waiting for bluetooth connection ...");
    uint64_t connectCounter = 0;
    while (!Blesrv::isConnected() && connectCounter < 5) {
        if (connectCounter % 5 == 0) {
            Blesrv::setLedStatus(BLUE_STATUS__ON);
            delay(100);
            Blesrv::setLedStatus(BLUE_STATUS_OFF);
            delay(100);
        } else {
            delay(400);
        }
        connectCounter++;
    }
    Serial.println("PP: setup - ... bluetooth connection established");
    for (uint8_t i = 0; i < 3; i++) {
        Blesrv::setLedStatus(BLUE_STATUS__ON);
        delay(100);
        Blesrv::setLedStatus(BLUE_STATUS_OFF);
        delay(100);
    }

    // set initial buffer size and position
    Blesrv::writeBuffSize();
    Blesrv::writePosition();
    Serial.println("PP: setup - ... initial values written");

#ifdef USE_DISPLAY
    Display::addMessage("begin switches");
#endif

    Switches::begin();  // initial regular RGB values, actually turns off RGB led when no switches are pressed

#ifdef USE_DISPLAY
    Display::addMessage("power motors a and b");
#endif

    Motors::motorA.powerup();
    Motors::motorB.powerup();
    delay(1000);

#ifdef USE_DISPLAY
    Display::addMessage("power motor z");
#endif

    Motors::motorZ.powerup();
    delay(1000);

#ifdef USE_DISPLAY
    Display::addMessage("power motor r");
#endif

    Motors::motorR.powerup();
    delay(1000);

#ifdef USE_DISPLAY
    Display::addMessage("begin driver");
#endif

    // start machine pulse (the driving interval) on core 0
    xTaskCreatePinnedToCore(driverBegin, "driver-begin", 100000, NULL, 1, NULL, 0);

    Serial.print("PP: setup - 3, ESP.getFreeHeap(): ");
    Serial.println(ESP.getFreeHeap());

    // delay(1000);
    // Display::depower();

    // Driver::pulse();
}

void loop() {

    // Serial.print("acceptMicros: ");
    // Serial.println(Device::acceptCount > 0 ? Device::acceptMicros / Device::acceptCount : 0);

    // if (loopCounter % 5 == 1) {
    //     for (uint16_t p = 0; p < 320; p++) {
    //         Motors::motorR.pulse();
    //         delay(1);
    //     }
    // }

#ifdef USE_DISPLAY
    if (loopCounter % 10 == 0) {
        Display::drawCounters();
    }
#endif

    // #ifdef USE_SERIAL
    //     Serial.print("switches: ");
    //     Serial.print(Switches::limitX.isPressed());
    //     Serial.print(Switches::limitY.isPressed());
    //     Serial.println(Switches::limitZ.isPressed());
    // #endif

    Switches::updateNeopixel();

    if (loopCounter % 5 == 0) {
        Blesrv::writePosition();
    }
    Blesrv::writeBuffSize();  // only writes when the current value is not equal to the last written value
    delay(100);

    loopCounter++;
}
