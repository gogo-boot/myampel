#include "SignalController.h"
#include "config/pins.h"
#include <Arduino.h>

static const char* TAG = "Signal";

void SignalController::begin() {
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_PRE_YELLOW_1, OUTPUT);
    pinMode(PIN_PRE_YELLOW_2, OUTPUT);
    pinMode(PIN_PRE_GREEN_1, OUTPUT);
    pinMode(PIN_PRE_GREEN_2, OUTPUT);
    allOff();
}

void SignalController::update() {
    if (_redTimerActive && redTimerExpired()) {
        _redTimerActive = false;
        setMain(MainSignal::GREEN);
        ESP_LOGI(TAG, "Auto-green timer expired");
    }
}

void SignalController::selfTest() {
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, HIGH);
    digitalWrite(PIN_PRE_YELLOW_1, HIGH);
    digitalWrite(PIN_PRE_YELLOW_2, HIGH);
    digitalWrite(PIN_PRE_GREEN_1, HIGH);
    digitalWrite(PIN_PRE_GREEN_2, HIGH);
}

void SignalController::blinkId(uint8_t id) {
    // Non-blocking blink handled via millis in a simple way:
    // Called once, blinks synchronously during IDENTIFY stage
    for (uint8_t i = 0; i < id; i++) {
        digitalWrite(PIN_LED_RED, HIGH);
        digitalWrite(PIN_LED_GREEN, HIGH);
        delay(IDENTIFY_BLINK_MS);
        allOff();
        delay(IDENTIFY_BLINK_MS);
    }
}

void SignalController::allOff() {
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_PRE_YELLOW_1, LOW);
    digitalWrite(PIN_PRE_YELLOW_2, LOW);
    digitalWrite(PIN_PRE_GREEN_1, LOW);
    digitalWrite(PIN_PRE_GREEN_2, LOW);
}

void SignalController::setMain(MainSignal state) {
    _main = state;
    applyMain();
    if (state == MainSignal::RED) {
        startRedTimer();
    }
}

void SignalController::setPre(PreSignal state) {
    _pre = state;
    applyPre();
}

void SignalController::startRedTimer() {
    _redTimerStart = millis();
    _redTimerActive = true;
}

bool SignalController::redTimerExpired() const {
    return millis() - _redTimerStart >= AUTO_GREEN_TIMER_MS;
}

void SignalController::applyMain() {
    digitalWrite(PIN_LED_RED, _main == MainSignal::RED ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN, _main == MainSignal::GREEN ? HIGH : LOW);
}

void SignalController::applyPre() {
    // YELLOW = next signal is RED → yellow pair on, green pair off
    // GREEN = next signal is GREEN → green pair on, yellow pair off
    if (_pre == PreSignal::YELLOW) {
        digitalWrite(PIN_PRE_YELLOW_1, HIGH);
        digitalWrite(PIN_PRE_YELLOW_2, HIGH);
        digitalWrite(PIN_PRE_GREEN_1, LOW);
        digitalWrite(PIN_PRE_GREEN_2, LOW);
    } else {
        digitalWrite(PIN_PRE_YELLOW_1, LOW);
        digitalWrite(PIN_PRE_YELLOW_2, LOW);
        digitalWrite(PIN_PRE_GREEN_1, HIGH);
        digitalWrite(PIN_PRE_GREEN_2, HIGH);
    }
}
