#include "ButtonManager.h"

ButtonManager::ButtonManager(const std::vector<uint8_t>& pins)
    : buttonPins(pins), lastStates(pins.size(), true) {}

void ButtonManager::begin() {
    for (uint8_t pin : buttonPins) {
        pinMode(pin, INPUT_PULLUP);
    }

    xTaskCreatePinnedToCore(
        buttonTask,
        "ButtonTask",
        2048,
        this,
        1,
        nullptr,
        1  // Core 1 para não competir com WiFi/BLE
    );
}

void ButtonManager::onClick(std::function<void(int)> callback) {
    clickCallback = callback;
}

void ButtonManager::onRelease(std::function<void(int)> callback) {
    releaseCallback = callback;
}

void ButtonManager::buttonTask(void* pvParameters) {
    ButtonManager* manager = static_cast<ButtonManager*>(pvParameters);

    while (true) {
        for (size_t i = 0; i < manager->buttonPins.size(); ++i) {
            uint8_t pin = manager->buttonPins[i];
            bool currentState = digitalRead(pin);  // HIGH = solto, LOW = pressionado

            if (manager->lastStates[i] != currentState) {
                manager->lastStates[i] = currentState;

                if (currentState == LOW && manager->clickCallback) {
                    manager->clickCallback(i);
                } else if (currentState == HIGH && manager->releaseCallback) {
                    manager->releaseCallback(i);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));  // Debounce + economia de CPU
    }
}