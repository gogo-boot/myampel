#include "TrainDetector.h"
#include "config/pins.h"
#include <Arduino.h>

static const char* TAG = "IR";

void TrainDetector::begin(uint8_t signalId) {
    _signalId = signalId;
    pinMode(PIN_IR_SENSOR, INPUT_PULLUP);
    ledcSetup(0, IR_CARRIER_HZ, 8);  // Channel 0, 38kHz, 8-bit
    ledcAttachPin(PIN_IR_TX, 0);
    ledcWrite(0, 0);  // Start off
}

void TrainDetector::update() {
    _triggered = false;
    uint32_t now = millis();

    // Check every 50ms
    if (now - _lastCheck < 50) return;
    _lastCheck = now;

    sendBurst();
    delayMicroseconds(200);  // Wait for reflection
    bool detected = readReflection();

    if (detected && (now - _lastDetect >= IR_DEBOUNCE_MS)) {
        _lastDetect = now;
        _triggered = true;
        ESP_LOGI(TAG, "Train detected (signal %d)", _signalId);
    }
}

void TrainDetector::sendBurst() {
    // Send unique code: N bursts where N = signal_id
    // Each burst is IR_BURST_MS of 38kHz followed by IR_GAP_MS silence
    for (uint8_t i = 0; i < _signalId; i++) {
        ledcWrite(0, 128);  // 50% duty = 38kHz on
        delay(IR_BURST_MS);
        ledcWrite(0, 0);    // Off
        if (i < _signalId - 1) delay(IR_GAP_MS);
    }
}

bool TrainDetector::readReflection() {
    // Phototransistor: LOW = IR light reflected back (train present)
    // Sample multiple times to confirm
    uint8_t lowCount = 0;
    for (uint8_t i = 0; i < 5; i++) {
        if (!digitalRead(PIN_IR_SENSOR)) lowCount++;
        delayMicroseconds(100);
    }
    return lowCount >= 3;  // Majority vote
}
