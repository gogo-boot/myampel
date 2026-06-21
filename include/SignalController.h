#pragma once
#include "signal_types.h"

class SignalController {
public:
    void begin();
    void update();
    void selfTest();
    void blinkId(uint8_t id);
    void allOff();

    void setMain(MainSignal state);
    void setPre(PreSignal state);
    void setSignalId(uint8_t id) { _id = id; }

    MainSignal mainState() const { return _main; }
    PreSignal preState() const { return _pre; }

    // Auto-green timer
    void startRedTimer();
    bool redTimerExpired() const;

private:
    void applyMain();
    void applyPre();

    MainSignal _main = MainSignal::GREEN;
    PreSignal _pre = PreSignal::GREEN;
    uint8_t _id = 1;
    uint32_t _redTimerStart = 0;
    bool _redTimerActive = false;
};
