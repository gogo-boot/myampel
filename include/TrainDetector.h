#pragma once
#include "signal_types.h"

class TrainDetector {
public:
    void begin(uint8_t signalId);
    void update();
    bool triggered() const { return _triggered; }

private:
    void sendBurst();
    bool readReflection();

    uint8_t _signalId = 1;
    bool _triggered = false;
    uint32_t _lastDetect = 0;
    uint32_t _lastCheck = 0;
};
