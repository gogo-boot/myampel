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
    updateButton(_green);
    updateButton(_red);

    // Both held detection
    _bothHeld = !digitalRead(PIN_BTN_GREEN) && !digitalRead(PIN_BTN_RED);
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

void ButtonHandler::updateButton(ButtonState& btn) {
    bool reading = digitalRead(btn.pin);  // LOW = pressed (active low)
    uint32_t now = millis();

    if (reading != btn.lastReading) {
        btn.lastChange = now;
    }
    btn.lastReading = reading;

    if (now - btn.lastChange < BUTTON_DEBOUNCE_MS) return;

    bool pressed = !reading;  // active low

    if (pressed && !btn.pressed) {
        // Just pressed
        btn.pressed = true;
        btn.pressStart = now;
    } else if (!pressed && btn.pressed) {
        // Just released
        btn.pressed = false;
        uint32_t duration = now - btn.pressStart;
        btn.event = (duration >= BUTTON_LONG_PRESS_MS)
            ? ButtonEvent::LONG_PRESS
            : ButtonEvent::SHORT_PRESS;
    }
}
