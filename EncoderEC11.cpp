#include "EncoderEC11.h"

EncoderEC11::EncoderEC11(uint8_t pinA, uint8_t pinB, uint8_t btnDir, uint8_t btnEsq)
    : pinA(pinA), pinB(pinB), btnDir(btnDir), btnEsq(btnEsq) {}

void EncoderEC11::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);

    lastState = (digitalRead(pinA) << 1) | digitalRead(pinB);
    lastPulse = 0;

    xTaskCreatePinnedToCore(
        encoderTask,
        "EncoderTask",
        2048,
        this,
        1,
        nullptr,
        1
    );
}

void EncoderEC11::onRotateRight(std::function<void(uint8_t)> callback) {
    callbackDir = callback;
}

void EncoderEC11::onRotateLeft(std::function<void(uint8_t)> callback) {
    callbackEsq = callback;
}

void EncoderEC11::encoderTask(void* pvParameters) {
    EncoderEC11* self = static_cast<EncoderEC11*>(pvParameters);

    while (true) {
        unsigned long now = millis();
        uint8_t state = (digitalRead(self->pinA) << 1) | digitalRead(self->pinB);

        if (state != self->lastState && (now - self->lastPulse >= pulseIntervalMs)) {
            bool pulseDetected = false;

            // Direita
            if ((self->lastState == 0 && state == 1) ||
                (self->lastState == 1 && state == 3) ||
                (self->lastState == 3 && state == 2) ||
                (self->lastState == 2 && state == 0)) {
                pulseDetected = true;
                if (state == 2 && self->callbackDir) self->callbackDir(self->btnDir);
            }
            // Esquerda
            else if ((self->lastState == 0 && state == 2) ||
                     (self->lastState == 2 && state == 3) ||
                     (self->lastState == 3 && state == 1) ||
                     (self->lastState == 1 && state == 0)) {
                pulseDetected = true;
                if (state == 1 && self->callbackEsq) self->callbackEsq(self->btnEsq);
            }

            if (pulseDetected) {
                self->lastState = state;
                self->lastPulse = now;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));  // Escaneia a cada 5ms
    }
}