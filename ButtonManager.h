#pragma once

#include <Arduino.h>
#include <vector>
#include <functional>

class ButtonManager {
public:
    ButtonManager(const std::vector<uint8_t>& pins);

    void begin();
    void onClick(std::function<void(int)> callback);
    void onRelease(std::function<void(int)> callback);

private:
    static void buttonTask(void* pvParameters);

    std::vector<uint8_t> buttonPins;
    std::vector<bool> lastStates;

    std::function<void(int)> clickCallback;
    std::function<void(int)> releaseCallback;
};