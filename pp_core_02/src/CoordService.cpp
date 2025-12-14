#include "CoordService.h"

BLEServer* CoordService::pServer;
BLEService* CoordService::pService;
BLECharacteristic* CoordService::pBuffSizeCharacteristic;
BLECharacteristic* CoordService::pBuffValsCharacteristic;
BLECharacteristic* CoordService::pPositionCharacteristic;

// Function to convert a struct to a byte array
// https://wokwi.com/projects/384215584338530305
template <typename T>
void serializeData(const T& inputStruct, uint8_t* outputBytes) {
    memcpy(outputBytes, &inputStruct, sizeof(T));
}

// Function to convert a byte array to a struct
// https://wokwi.com/projects/384215584338530305
template <typename T>
void deserializeData(const uint8_t* inputBytes, uint16_t offset, T& outputStruct) {
    memcpy(&outputStruct, inputBytes + offset, sizeof(T));
}

class CoordServiceCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        // do nothing
        // Serial.println("something connected, count: " + pServer->getConnectedCount());
    };
    void onDisconnect(BLEServer* pServer) {
        // Serial.println("something disconnected, count: " + pServer->getConnectedCount());
        // TODO :: clear all coordinates
        BLEDevice::startAdvertising();
    }
};

class BuffValueCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        size_t pDataLength = pCharacteristic->getLength();
        // Serial.print("pDataLength: ");
        // Serial.print(String(pDataLength));
        // Serial.print(", core: ");
        // Serial.println(xPortGetCoreID());

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        block_planxy_____t blockPlanxy;
        for (uint16_t newValueIndex = 0; newValueIndex < COMMAND_BUFF_VALS_SIZE * sizeof(block_planxy_____t); newValueIndex += sizeof(block_planxy_____t)) {

            deserializeData(newValue, newValueIndex, blockPlanxy);

            // Serial.print("blockPlanar, x:");
            // Serial.print(String(blockPlanar.x, 3));
            // Serial.print(", y: ");
            // Serial.println(String(blockPlanar.y, 3));

            Coords::addBlock(blockPlanxy);
        }
        CoordService::setBuffSize();  // update buffer size after reading
    }
};

bool CoordService::isConnected() {
    return CoordService::pServer->getConnectedCount() > 0;
}

bool CoordService::begin() {

    BLEDevice::init(BTLE_DEVICE_NAME);
    CoordService::pServer = BLEDevice::createServer();
    CoordService::pServer->setCallbacks(new CoordServiceCallbacks());

    CoordService::pService = pServer->createService(COMMAND_SERVICE___UUID);

    CoordService::pBuffSizeCharacteristic = CoordService::pService->createCharacteristic(COMMAND_BUFF_SIZE_UUID, BLECharacteristic::PROPERTY_READ);  // | BLECharacteristic::PROPERTY_NOTIFY
    CoordService::pBuffSizeCharacteristic->addDescriptor(new BLEDescriptor(DESCRIB_BUFF_SIZE_UUID, sizeof(int)));
    CoordService::pBuffSizeCharacteristic->addDescriptor(new BLE2902());

    CoordService::pBuffValsCharacteristic = CoordService::pService->createCharacteristic(COMMAND_BUFF_VALS_UUID, BLECharacteristic::PROPERTY_WRITE);
    CoordService::pBuffValsCharacteristic->addDescriptor(new BLEDescriptor(DESCRIB_BUFF_VALS_UUID, sizeof(block_planxy_____t) * COMMAND_BUFF_VALS_SIZE));
    CoordService::pBuffValsCharacteristic->setCallbacks(new BuffValueCallbacks());

    CoordService::pPositionCharacteristic = CoordService::pService->createCharacteristic(COMMAND_POSITION__UUID, BLECharacteristic::PROPERTY_READ);  // | BLECharacteristic::PROPERTY_NOTIFY
    CoordService::pPositionCharacteristic->addDescriptor(new BLEDescriptor(DESCRIB_POSITION__UUID, sizeof(coord_planxy_____t)));
    CoordService::pBuffSizeCharacteristic->addDescriptor(new BLE2902());

    CoordService::pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(COMMAND_SERVICE___UUID);
    pAdvertising->addServiceUUID(COMMAND_BUFF_VALS_UUID);
    pAdvertising->addServiceUUID(COMMAND_POSITION__UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    return true;
}

void CoordService::setBuffSize() {
    if (CoordService::isConnected()) {
        int buffSize = (int)Coords::getBuffSize();
        // Serial.print("buffSize: ");
        // Serial.println(String(buffSize));
        CoordService::pBuffSizeCharacteristic->setValue(buffSize);
    }
}

void CoordService::setPosition() {
    if (CoordService::isConnected()) {
        coord_corexy_____t curCorexy = {0, 0, 0};  // Motors::getCurCorexy();
        coord_planxy_____t curPlanar = Coords::corexyToPlanxy(curCorexy);
        // Serial.print("x: ");
        // Serial.print(String(curPlanar.x, 3));
        // Serial.print(", y: ");
        // Serial.print(String(curPlanar.y, 3));
        // Serial.print(", z: ");
        // Serial.println(String(curPlanar.z, 3));
        uint8_t outValue[sizeof(curPlanar)];
        serializeData(curPlanar, outValue);
        CoordService::pPositionCharacteristic->setValue(outValue, sizeof(curPlanar));
    }
}
