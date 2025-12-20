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
};

#endif