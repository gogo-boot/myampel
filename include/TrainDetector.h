#pragma once
#include "signal_types.h"
#include <cstdint>

class TrainDetector {
public:
    void begin();
    void update();
    bool triggered() const { return _triggered; }

private:
    bool _triggered = false;
    bool _lastState = true;  // HIGH = no train (pull-up)
    uint32_t _lastChange = 0;
};
