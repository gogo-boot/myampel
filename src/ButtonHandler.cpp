#include "ButtonHandler.h"
#include "config/pins.h"
#include <Arduino.h>

void ButtonHandler::begin() {
    _green.pin = PIN_BTN_GREEN;
    _red.pin = PIN_BTN_RED;
    pinMode(PIN_BTN_GREEN, INPUT_PULLUP);
    pinMode(PIN_BTN_RED, INPUT_PULLUP);
}

void ButtonHandler::update() {
    _green.event = ButtonEvent::NONE;
    _red.event = ButtonEvent::NONE;
    updateBtn(_green);
    updateBtn(_red);

    bool bothNow = !digitalRead(PIN_BTN_GREEN) && !digitalRead(PIN_BTN_RED);
    if (bothNow && !_bothHeld) {
        _bothHeld = true;
        _bothHeldStart = millis();
        _bothHeldFired = false;
    } else if (!bothNow) {
        _bothHeld = false;
        _bothHeldFired = false;
    }
}

bool ButtonHandler::bothHeldTriggered() {
    if (_bothHeld && !_bothHeldFired &&
        (millis() - _bothHeldStart >= BUTTON_LONG_PRESS_MS)) {
        _bothHeldFired = true;
        return true;
    }
    return false;
}

ButtonEvent ButtonHandler::greenEvent() {
    ButtonEvent e = _green.event;
    _green.event = ButtonEvent::NONE;
    return e;
}

ButtonEvent ButtonHandler::redEvent() {
    ButtonEvent e = _red.event;
    _red.event = ButtonEvent::NONE;
    return e;
}

void ButtonHandler::updateBtn(BtnState& b) {
    bool reading = digitalRead(b.pin);
    uint32_t now = millis();

    if (reading != b.lastReading) b.lastChange = now;
    b.lastReading = reading;
    if (now - b.lastChange < BUTTON_DEBOUNCE_MS) return;

    bool pressed = !reading;
    if (pressed && !b.pressed) {
        b.pressed = true;
        b.pressStart = now;
    } else if (!pressed && b.pressed) {
        b.pressed = false;
        uint32_t dur = now - b.pressStart;
        b.event = (dur >= BUTTON_LONG_PRESS_MS) ? ButtonEvent::LONG_PRESS : ButtonEvent::SHORT_PRESS;
    }
}
