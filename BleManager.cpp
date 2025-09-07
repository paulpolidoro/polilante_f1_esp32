#include "BleManager.h"
#include <Arduino.h>
#include "Debug.h"

std::function<void(uint8_t*, size_t)> BleManager::userCallback = nullptr;

BleManager::BleManager(const char* serviceUUID, const char* characteristicUUID)
    : serviceUUID(serviceUUID), characteristicUUID(characteristicUUID), connected(false),
      pClient(nullptr), pRemoteCharacteristic(nullptr) {}

bool BleManager::connect() {
    DEBUG_PRINTLN("Tentando conectar ao servidor BLE...");
    BLEAddress serverAddress(SERVER_MAC);
    pClient = BLEDevice::createClient();

    if (!pClient->connect(serverAddress)) {
        DEBUG_PRINTLN("Falha na conexão BLE.");
        return false;
    }

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        DEBUG_PRINTLN("Serviço não encontrado.");
        return false;
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(characteristicUUID);
    if (pRemoteCharacteristic == nullptr) {
        DEBUG_PRINTLN("Característica não encontrada.");
        return false;
    }

    pRemoteCharacteristic->registerForNotify(notifyCallback);
    DEBUG_PRINTLN("Conectado e inscrito para notificações BLE.");
    connected = true;
    return true;
}

bool BleManager::isConnected() const {
    return connected && pClient && pClient->isConnected();
}

void BleManager::disconnect() {
    if (pClient && pClient->isConnected()) {
        pClient->disconnect();
    }
    connected = false;
}

bool BleManager::send(const uint8_t* data, size_t length) {
    if (isConnected() && pRemoteCharacteristic) {
        pRemoteCharacteristic->writeValue(const_cast<uint8_t*>(data), length, false);
        return true;
    }
    return false;
}

void BleManager::onReceive(std::function<void(uint8_t*, size_t)> callback) {
    userCallback = callback;
}

void BleManager::notifyCallback(BLERemoteCharacteristic* pChar, uint8_t* data, size_t length, bool isNotify) {
    if (userCallback) {
        userCallback(data, length);
    }
}