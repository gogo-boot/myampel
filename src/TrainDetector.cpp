#include "TrainDetector.h"
#include "config/pins.h"
#include <Arduino.h>

void TrainDetector::begin() {
    pinMode(PIN_IR_SENSOR, INPUT_PULLUP);
}

void TrainDetector::update() {
    _triggered = false;
    bool reading = digitalRead(PIN_IR_SENSOR);
    uint32_t now = millis();

    if (reading != _lastState) {
        if (now - _lastChange >= IR_DEBOUNCE_MS) {
            _lastState = reading;
            _lastChange = now;
            // LOW = beam broken = train detected
            if (!reading) {
                _triggered = true;
            }
        }
    }
}
