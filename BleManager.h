#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEAddress.h>
#include <functional>

class BleManager {
public:
    BleManager(const char* serviceUUID, const char* characteristicUUID);
    
    bool connect();
    bool isConnected() const;
    void disconnect();
    bool send(const uint8_t* data, size_t length);
    void onReceive(std::function<void(uint8_t*, size_t)> callback);
    void onDisconnect(std::function<void()> callback);


private:
    static void notifyCallback(BLERemoteCharacteristic* pChar, uint8_t* data, size_t length, bool isNotify);

    static void connectionMonitorTask(void* pvParameters);
    std::function<void()> disconnectCallback;
    bool lastConnectionState = false;

    static std::function<void(uint8_t*, size_t)> userCallback;

    BLEClient* pClient;
    BLERemoteCharacteristic* pRemoteCharacteristic;
    bool connected;

    const char* serviceUUID;
    const char* characteristicUUID;

    static constexpr const char* SERVER_MAC = "cc:ba:97:09:40:fa";
};