#pragma once
#include "signal_types.h"
#include <cstdint>

class ButtonHandler {
public:
    void begin();
    void update();

    ButtonEvent greenEvent();
    ButtonEvent redEvent();

    bool bothHeld() const { return _bothHeld; }

private:
    struct ButtonState {
        uint8_t pin = 0;
        bool lastReading = true;
        bool stable = true;
        uint32_t lastChange = 0;
        uint32_t pressStart = 0;
        bool pressed = false;
        ButtonEvent event = ButtonEvent::NONE;
    };

    void updateButton(ButtonState& btn);

    ButtonState _green;
    ButtonState _red;
    bool _bothHeld = false;
};
