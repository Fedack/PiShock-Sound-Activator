#include "ble_manager.h"
#include "Arduino.h"

const char *BLEManager::SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char *BLEManager::NOISE_CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

class BLEManager::ServerCallbacks : public BLEServerCallbacks
{
private:
    BLEManager *bleManager;

public:
    ServerCallbacks(BLEManager *manager) : bleManager(manager) {}

    void onConnect(BLEServer *pServer) override
    {
        bleManager->deviceConnected = true;
        Serial.println("BLE Device Connected");
        BLEDevice::getAdvertising()->stop();
    }

    void onDisconnect(BLEServer *pServer) override
    {
        bleManager->deviceConnected = false;
        Serial.println("BLE Device Disconnected");
        BLEDevice::startAdvertising();
    }
};

void BLEManager::init()
{
    // Initialize BLE
    BLEDevice::init("pishock-squeakgate");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create BLE Characteristic for noise level
    pNoiseCharacteristic = pService->createCharacteristic(
        NOISE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY);

    // Create a BLE Descriptor
    pNoiseCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
}

void BLEManager::updateNoiseLevel(int noiseLevel)
{
    if (deviceConnected)
    {
        String noiseStr = String(noiseLevel);
        pNoiseCharacteristic->setValue(noiseStr.c_str());
        pNoiseCharacteristic->notify();
    }
}
