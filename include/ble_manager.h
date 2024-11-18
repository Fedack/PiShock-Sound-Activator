#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLEManager
{
private:
    BLEServer *pServer = nullptr;
    BLECharacteristic *pNoiseCharacteristic = nullptr;
    bool deviceConnected = false;

    // UUIDs for our service and characteristic
    static const char *SERVICE_UUID;
    static const char *NOISE_CHARACTERISTIC_UUID;

    // Forward declaration of the callback class
    class ServerCallbacks;

public:
    void init();
    void updateNoiseLevel(int noiseLevel);
    bool isConnected() { return deviceConnected; }

    // Friend declaration so ServerCallbacks can access deviceConnected
    friend class ServerCallbacks;
};

#endif
