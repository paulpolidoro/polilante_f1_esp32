#include "TachometerManager.h"

TachometerManager::TachometerManager(uint8_t pin, uint8_t numLeds)
    : strip(numLeds, pin, NEO_GRB + NEO_KHZ800), numLeds(numLeds) {}

void TachometerManager::begin() {
    strip.begin();
    strip.setBrightness(map(brightness, 0, 100, 0, 255));
    strip.show();

    // Cria a task de flash
    xTaskCreatePinnedToCore(
        flashTask,
        "FlashTask",
        2048,
        this,
        1,
        nullptr,
        1  // Core 1 para não competir com WiFi/BLE
    );
}

void TachometerManager::setFlashPoint(uint8_t percent) {
    flashPoint = constrain(percent, 0, 100);
}

void TachometerManager::setBright(uint8_t percent) {
    brightness = constrain(percent, 0, 100);
    strip.setBrightness(map(brightness, 0, 100, 0, 255));
    strip.show();
}

void TachometerManager::updateRPM(uint8_t percent) {
    currentRPM = constrain(percent, 0, 100);

    if (currentRPM >= flashPoint) {
        flashing = true;
    } else {
        flashing = false;
        showRPM();
    }
}

void TachometerManager::showRPM() {
    uint8_t ledsOn = map(currentRPM, 0, 100, 0, numLeds);

    for (uint8_t i = 0; i < numLeds; ++i) {
        uint8_t reversedIndex = numLeds - 1 - i;

        if (i < ledsOn) {
            strip.setPixelColor(reversedIndex, getColorByIndex(reversedIndex));
        } else {
            strip.setPixelColor(reversedIndex, 0);  // Apagado
        }
    }

    strip.show();
}

void TachometerManager::flashTask(void* pvParameters) {
    TachometerManager* self = static_cast<TachometerManager*>(pvParameters);
    bool flashState = false;

    while (true) {
        if (self->flashing) {
            flashState = !flashState;

            for (uint8_t i = 0; i < self->numLeds; ++i) {
                uint8_t reversedIndex = self->numLeds - 1 - i;
                self->strip.setPixelColor(reversedIndex, flashState ? self->strip.Color(255, 0, 0) : 0);
            }

            self->strip.show();
            vTaskDelay(pdMS_TO_TICKS(300));
        } else {
            vTaskDelay(pdMS_TO_TICKS(50));  // Aguarda sem piscar
        }
    }
}

uint32_t TachometerManager::getColorByIndex(uint8_t index) {
    // Cores invertidas: vermelho à esquerda, azul no meio, verde à direita
    if (index < 2) return strip.Color(255, 0, 0);       // Vermelho
    if (index < 5) return strip.Color(0, 0, 255);       // Azul
    return strip.Color(0, 255, 0);                      // Verde
}

void TachometerManager::startBlinkLastLedBlue() {
    blinkingLastLed = true;

    xTaskCreatePinnedToCore(
        blinkLastLedTask,
        "BlinkLastLedTask",
        2048,
        this,
        1,
        nullptr,
        1
    );
}

void TachometerManager::stopBlinkLastLed() {
    blinkingLastLed = false;
}

void TachometerManager::blinkLastLedTask(void* pvParameters) {
    TachometerManager* self = static_cast<TachometerManager*>(pvParameters);
    bool state = false;
    uint8_t lastIndex = self->numLeds - 1;

    while (self->blinkingLastLed) {
        state = !state;
        self->strip.setPixelColor(lastIndex, state ? self->strip.Color(0, 0, 255) : 0);
        self->strip.show();
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // Apaga LED ao encerrar
    self->strip.setPixelColor(lastIndex, 0);
    self->strip.show();
    vTaskDelete(nullptr);
}