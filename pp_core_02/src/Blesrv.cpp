#include "Blesrv.h"

int Blesrv::buffSizeLast = -1;
BLEServer* Blesrv::pServer;
BLEService* Blesrv::pService;
BLECharacteristic* Blesrv::pBuffSizeCharacteristic;
BLECharacteristic* Blesrv::pBuffValsCharacteristic;
BLECharacteristic* Blesrv::pPositionCharacteristic;

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

class BlesrvCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        // do nothing
        // Serial.println("something connected, count: " + pServer->getConnectedCount());
    };
    void onDisconnect(BLEServer* pServer) {
        // Serial.println("something disconnected, count: " + pServer->getConnectedCount());
        // TODO :: clear all coordinates
        // BLEDevice::startAdvertising();
        esp_restart();
    }
};

class BuffValueCallbacks : public BLECharacteristicCallbacks {

    void onWrite(BLECharacteristic* pCharacteristic) {

        size_t pDataLength = pCharacteristic->getLength();
        // Serial.print("pDataLength: ");
        // Serial.println(String(pDataLength));
        // Serial.print(", core: ");
        // Serial.println(xPortGetCoreID());

        uint8_t* newValue = (uint8_t*)pCharacteristic->getData();
        block_planxy_f___t blockPlanxy_f;
        block_planxy_i64_t blockPlanxy_i;
        for (uint16_t newValueIndex = 0; newValueIndex < COMMAND_BUFF_VALS_SIZE * sizeof(block_planxy_f___t); newValueIndex += sizeof(block_planxy_f___t)) {
            deserializeData(newValue, newValueIndex, blockPlanxy_f);
            blockPlanxy_i = Coords::planxyToPlanxy(blockPlanxy_f);
            Coords::addBlock(blockPlanxy_i);
        }
        Blesrv::writeBuffSize();  // update buffer size after having read new blocks
    }
};

bool Blesrv::isConnected() {
    return Blesrv::pServer->getConnectedCount() > 0;
}

bool Blesrv::begin() {

    BLEDevice::init(BTLE_DEVICE_NAME);

    // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P18);
    // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
    // esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);

    // int pwrAdv = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV);
    // int pwrScan = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_SCAN);
    // int pwrDef = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_DEFAULT);
    // Serial.println("Power Settings: (ADV,SCAN,DEFAULT)");  // all should show index7, aka +9dbm
    // Serial.println(pwrAdv);
    // Serial.println(pwrScan);
    // Serial.println(pwrDef);

    Blesrv::pServer = BLEDevice::createServer();
    Blesrv::pServer->setCallbacks(new BlesrvCallbacks());

    Blesrv::pService = pServer->createService(COMMAND_SERVICE___UUID);

    Blesrv::pBuffSizeCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_BUFF_SIZE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);  // | BLECharacteristic::PROPERTY_NOTIFY
    // int buffSizeInitValue = 0;
    // Blesrv::pBuffSizeCharacteristic->setValue(buffSizeInitValue);
    // Blesrv::pBuffSizeCharacteristic->addDescriptor(new BLEDescriptor(DESCRIB_BUFF_SIZE_UUID, sizeof(int)));
    Blesrv::pBuffSizeCharacteristic->addDescriptor(new BLE2902());

    uint16_t buffValsLength = sizeof(block_planxy_f___t) * COMMAND_BUFF_VALS_SIZE;
    // Serial.print("buffValsLength: ");
    // Serial.println(String(buffValsLength));
    Blesrv::pBuffValsCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_BUFF_VALS_UUID, BLECharacteristic::PROPERTY_WRITE);
    // uint8_t buffValsInitValue[buffValsLength];
    // BLEUUID buffValsUuid = BLEUUID(DESCRIB_BUFF_VALS_UUID);
    // Blesrv::pBuffValsCharacteristic->setValue(buffValsInitValue, buffValsLength);
    // Blesrv::pBuffValsCharacteristic->addDescriptor(new BLEDescriptor(buffValsUuid, buffValsLength));
    Blesrv::pBuffValsCharacteristic->setCallbacks(new BuffValueCallbacks());

    Blesrv::pPositionCharacteristic = Blesrv::pService->createCharacteristic(COMMAND_POSITION__UUID, BLECharacteristic::PROPERTY_READ);  // | BLECharacteristic::PROPERTY_NOTIFY
    // Blesrv::pPositionCharacteristic->addDescriptor(new BLEDescriptor(DESCRIB_POSITION__UUID, sizeof(coord_planxy_f___t)));
    // Blesrv::pBuffSizeCharacteristic->addDescriptor(new BLE2902());

    Blesrv::pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(COMMAND_SERVICE___UUID);
    pAdvertising->addServiceUUID(COMMAND_BUFF_VALS_UUID);
    pAdvertising->addServiceUUID(COMMAND_POSITION__UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    return true;
}

bool Blesrv::writeBuffSize() {
    if (Blesrv::isConnected()) {
        int buffSizeCurr = (int)Coords::getBuffSize();
        if (buffSizeCurr != Blesrv::buffSizeLast) {
            Blesrv::pBuffSizeCharacteristic->setValue(buffSizeCurr);
            Blesrv::buffSizeLast = buffSizeCurr;
            return true;
        }
    }
    return false;
}

bool Blesrv::writePosition() {
    if (Blesrv::isConnected()) {
        coord_corexy_____t curCorexy = Motors::getCurCorexy();
        coord_planxy_i64_t curPlan_i = Coords::corexyToPlanxy(curCorexy);
        coord_planxy_f___t curPlan_f = Coords::planxyToPlanxy(curPlan_i);
        uint8_t outValue[sizeof(curPlan_f)];
        serializeData(curPlan_f, outValue);
        Blesrv::pPositionCharacteristic->setValue(outValue, sizeof(curPlan_f));
        return true;
    } else {
        return false;
    }
}
