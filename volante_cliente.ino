#include <Arduino.h>
#include "BleManager.h"
#include "ButtonManager.h"
#include "TachometerManager.h"
#include "EncoderEC11.h"
#include "Debug.h"

// 🔧 Configurações
#define LED_PIN 23  // Pino da fita LED
#define BLE_SERVICE_UUID "0000AB10-0000-1000-8000-00805F9B34FB"
#define BLE_CHARACTERISTIC_UUID "0000AB11-0000-1000-8000-00805F9B34FB"

std::vector<uint8_t> botoes = {4, 18, 5, 19, 12, 14, 13, 17, 21, 22, 27, 15};

// 🧠 Instâncias dos managers
BleManager ble(BLE_SERVICE_UUID, BLE_CHARACTERISTIC_UUID);
ButtonManager buttonManager(botoes);
TachometerManager tachometer(LED_PIN);

EncoderEC11 enc1 = {25, 26, 13, 14};
EncoderEC11 enc2 = {32, 33, 15, 16};

int valor = 0;

void handleBleData(uint8_t* data, size_t length) {
  if (length % 2 != 0) {
    DEBUG_PRINTLN("Tamanho inválido do pacote.");
    return;
  }

  for (size_t i = 0; i < length; i += 2) {
    uint8_t id = data[i];
    uint8_t value = data[i + 1];

   switch (id) {
      case 0x01:
        tachometer.updateRPM(value);
        break;
      case 0x02:
        tachometer.setFlashPoint(value);
        break;
      case 0x03:
         tachometer.setBright(value);
        break;
      default:
        DEBUG_PRINTLN("ID desconhecido: " + String(id));
        break;
    }
  }
}

void encoderButtonSimulate(uint8_t btnNum) {
    uint8_t* payload = new uint8_t[2]{ btnNum, 1 };
    ble.send(payload, 2);  // Envia botão pressionado

    xTaskCreatePinnedToCore(
        [](void* pvParameters) {
            uint8_t btn = *static_cast<uint8_t*>(pvParameters);
            delete static_cast<uint8_t*>(pvParameters);

            vTaskDelay(pdMS_TO_TICKS(100));  // Aguarda 100ms sem travar

            uint8_t payloadRelease[2] = { btn, 0 };
            ble.send(payloadRelease, sizeof(payloadRelease));

            vTaskDelete(nullptr);  // Finaliza a task
        },
        "AutoReleaseTask",
        2048,
        new uint8_t(btnNum),  // Passa o botão como parâmetro
        1,
        nullptr,
        1
    );
}

void setup() {
  if(DEBUG_MODE) {
    Serial.begin(115200);
  }

  // 🌈 Tachômetro
  tachometer.begin();
  tachometer.setBright(2);
  tachometer.setFlashPoint(90);

  tachometer.startBlinkLastLedBlue();

  BLEDevice::init("Polilante F1");

  // 📲 BLE
  ble.onReceive(handleBleData);
  while (!ble.connect()) {
    DEBUG_PRINTLN("Tentando reconectar BLE...");
    delay(5000);
  }

  tachometer.stopBlinkLastLed();

  // 🔘 Botões
  buttonManager.onClick([](int index) {
    DEBUG_PRINTLN("Botão pressionado: " + String(index));
    uint8_t payload[2] = { static_cast<uint8_t>(index), 1 };
    ble.send(payload, sizeof(payload));
  });

  buttonManager.onRelease([](int index) {
    DEBUG_PRINTLN("Botão liberado: " + String(index));

    uint8_t payload[2] = { static_cast<uint8_t>(index), 0 };
    ble.send(payload, sizeof(payload));
  });

  buttonManager.begin();

  enc1.begin();
  enc2.begin();

  enc1.onRotateRight([](uint8_t btn) {
    encoderButtonSimulate(btn);
    DEBUG_PRINTLN("Giro para direita! Botão: " + String(btn));
  });

  enc1.onRotateLeft([](uint8_t btn) {
    encoderButtonSimulate(btn);
    DEBUG_PRINTLN("Giro para esquerda! Botão: " + String(btn));
  });

  enc2.onRotateRight([](uint8_t btn) {
    encoderButtonSimulate(btn);
    DEBUG_PRINTLN("Giro para direita! Botão: " + String(btn));
  });

  enc2.onRotateLeft([](uint8_t btn) {
    encoderButtonSimulate(btn);
    DEBUG_PRINTLN("Giro para esquerda! Botão: " + String(btn));
  });

}

void loop() {

}