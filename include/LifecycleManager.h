#pragma once
#include "signal_types.h"

class SignalController;
class ButtonHandler;
class EspNowManager;
class TrainDetector;

class LifecycleManager {
public:
    void begin(SignalController& signal, ButtonHandler& buttons,
               EspNowManager& comm, TrainDetector& detector);
    void update();
    Stage stage() const { return _stage; }

private:
    void enterBoot();
    void enterConfig();
    void enterIdentify();
    void enterConnect();
    void enterRunning();
    void enterSleep();

    Stage _stage = Stage::BOOT;
    SignalController* _signal = nullptr;
    ButtonHandler* _buttons = nullptr;
    EspNowManager* _comm = nullptr;
    TrainDetector* _detector = nullptr;
    uint8_t _signalId = 1;
    uint32_t _stageStart = 0;
    uint32_t _lastActivity = 0;
};
