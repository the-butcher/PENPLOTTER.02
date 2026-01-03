#ifndef Blesrv_h
#define Blesrv_h

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "Coords.h"
#include "Define.h"
#include "Motors.h"

typedef enum {
    BLUE_STATUS__ON,
    BLUE_STATUS_OFF
} blue_led_status___e;

const gpio_num_t PIN_LED_BLUE = GPIO_NUM_44;

class Blesrv {
   private:
    static int buffSizeLast;
    static BLEServer* pServer;
    static BLEService* pService;
    static BLECharacteristic* pBuffSizeCharacteristic;
    static BLECharacteristic* pBuffValsCharacteristic;
    static BLECharacteristic* pPositionCharacteristic;

   public:
    static bool begin();
    static bool isConnected();
    static bool writeBuffSize();
    static bool writePosition();
    static void setLedStatus(blue_led_status___e status);
};

#endif