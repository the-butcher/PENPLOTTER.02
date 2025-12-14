#ifndef CoordService_h
#define CoordService_h

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "Coords.h"
#include "Define.h"
#include "Motors.h"
#include "Types.h"

class CoordService {
   private:
    static BLEServer* pServer;
    static BLEService* pService;
    static BLECharacteristic* pBuffSizeCharacteristic;
    static BLECharacteristic* pBuffValsCharacteristic;
    static BLECharacteristic* pPositionCharacteristic;

   public:
    static bool begin();
    static bool isConnected();
    static void setBuffSize();
    static void setPosition();
};

#endif