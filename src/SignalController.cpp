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
    if (_redTimerActive && (millis() - _redTimerStart >= AUTO_GREEN_TIMER_MS)) {
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

void SignalController::identifyFlash() {
    digitalWrite(PIN_LED_GREEN, HIGH);
    delay(IDENTIFY_DURATION_MS);
    digitalWrite(PIN_LED_GREEN, LOW);
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
    if (_main != state) {
        _main = state;
        _mainChanged = true;
        applyMain();
        if (state == MainSignal::RED) {
            _redTimerStart = millis();
            _redTimerActive = true;
        } else {
            _redTimerActive = false;
        }
        ESP_LOGI(TAG, "Main → %s", state == MainSignal::RED ? "RED" : "GREEN");
    }
}

void SignalController::setPre(PreSignal state) {
    if (_pre != state) {
        _pre = state;
        applyPre();
    }
}

void SignalController::notifyBlockClear() {
    if (_main == MainSignal::RED) {
        setMain(MainSignal::GREEN);
        ESP_LOGI(TAG, "Block clear → GREEN");
    }
}

void SignalController::applyMain() {
    digitalWrite(PIN_LED_RED, _main == MainSignal::RED ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN, _main == MainSignal::GREEN ? HIGH : LOW);
}

void SignalController::applyPre() {
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
