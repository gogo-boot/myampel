#pragma once
#include "signal_types.h"

class ButtonHandler {
public:
    void begin();
    void update();
    ButtonEvent greenEvent();
    ButtonEvent redEvent();
    bool bothHeld() const { return _bothHeld; }
    bool bothHeldTriggered();

private:
    struct BtnState {
        uint8_t pin = 0;
        bool lastReading = true;
        bool pressed = false;
        uint32_t lastChange = 0;
        uint32_t pressStart = 0;
        ButtonEvent event = ButtonEvent::NONE;
    };
    void updateBtn(BtnState& b);

    BtnState _green;
    BtnState _red;
    bool _bothHeld = false;
    bool _bothHeldFired = false;
    uint32_t _bothHeldStart = 0;
};
