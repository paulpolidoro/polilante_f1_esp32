#pragma once

#include <Adafruit_NeoPixel.h>

class TachometerManager {
public:
    TachometerManager(uint8_t pin, uint8_t numLeds = 8);

    void begin();
    void setFlashPoint(uint8_t percent);     // Define ponto de flash (0–100)
    void updateRPM(uint8_t percent);         // Atualiza RPM atual (0–100)
    void setBright(uint8_t percent);         // Define brilho geral (0–100)

    void startBlinkLastLedBlue();           // Inicia piscar azul no último LED
    void stopBlinkLastLed();                // Para piscar azul

private:
    void showRPM();                          // Mostra RPM com LEDs
    uint32_t getColorByIndex(uint8_t index); // Cor por posição invertida

    static void flashTask(void* pvParameters);       // Task dedicada ao flash
    static void blinkLastLedTask(void* pvParameters); // Task para piscar último LED

    Adafruit_NeoPixel strip;
    uint8_t numLeds;
    uint8_t flashPoint = 90;
    uint8_t currentRPM = 0;
    uint8_t brightness = 30;

    bool flashing = false;
    bool blinkingLastLed = false;
};
