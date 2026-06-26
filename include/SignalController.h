#pragma once
#include "signal_types.h"

class SignalController {
public:
    void begin();
    void update();
    void selfTest();
    void identifyFlash();
    void allOff();

    void setMain(MainSignal state);
    void setPre(PreSignal state);
    void setSignalId(uint8_t id) { _id = id; }
    void notifyBlockClear();

    MainSignal mainState() const { return _main; }
    PreSignal preState() const { return _pre; }
    uint8_t signalId() const { return _id; }
    bool mainChanged() const { return _mainChanged; }
    void clearMainChanged() { _mainChanged = false; }

private:
    void applyMain();
    void applyPre();

    MainSignal _main = MainSignal::GREEN;
    PreSignal _pre = PreSignal::GREEN;
    uint8_t _id = 1;
    uint32_t _redTimerStart = 0;
    bool _redTimerActive = false;
    bool _mainChanged = false;
};
