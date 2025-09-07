#pragma once

#include <Arduino.h>
#include <functional>

class EncoderEC11 {
public:
    EncoderEC11(uint8_t pinA, uint8_t pinB, uint8_t btnDir = 0, uint8_t btnEsq = 0);
    void begin();

    void onRotateRight(std::function<void(uint8_t)> callback);
    void onRotateLeft(std::function<void(uint8_t)> callback);

private:
    static void encoderTask(void* pvParameters);

    uint8_t pinA, pinB;
    uint8_t btnDir, btnEsq;

    uint8_t lastState = 0;
    unsigned long lastPulse = 0;

    std::function<void(uint8_t)> callbackDir;
    std::function<void(uint8_t)> callbackEsq;

    static const uint16_t pulseIntervalMs = 100;
};