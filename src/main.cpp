#include <Arduino.h>
#include "LifecycleManager.h"
#include "SignalController.h"
#include "ButtonHandler.h"
#include "EspNowManager.h"
#include "TrainDetector.h"

static LifecycleManager lifecycle;
static SignalController signalCtrl;
static ButtonHandler buttons;
static EspNowManager comm;
static TrainDetector detector;

static uint32_t lastHeartbeat = 0;

void handleRunning() {
    if (lifecycle.stage() != Stage::RUNNING) return;

    // Train detection → RED
    if (detector.triggered()) {
        signalCtrl.setMain(MainSignal::RED);
        comm.sendState(1, MainSignal::RED);
    }

    // Button: green → GREEN
    if (buttons.greenEvent() == ButtonEvent::SHORT_PRESS) {
        signalCtrl.setMain(MainSignal::GREEN);
        comm.sendState(1, MainSignal::GREEN);
    }

    // Button: red → RED
    if (buttons.redEvent() == ButtonEvent::SHORT_PRESS) {
        signalCtrl.setMain(MainSignal::RED);
        comm.sendState(1, MainSignal::RED);
    }

    // Auto-green timer expired → broadcast
    if (signalCtrl.mainState() == MainSignal::GREEN && signalCtrl.redTimerExpired()) {
        comm.sendState(1, MainSignal::GREEN);
    }

    // Receive peer state → update pre-signal
    if (comm.received()) {
        PreSignal pre = (comm.peerMainState() == MainSignal::RED)
            ? PreSignal::YELLOW : PreSignal::GREEN;
        signalCtrl.setPre(pre);
    }

    // Heartbeat broadcast every 2s
    uint32_t now = millis();
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = now;
        comm.sendState(1, signalCtrl.mainState());
    }
}

void setup() {
    Serial.begin(115200);
    lifecycle.begin(signalCtrl, buttons, comm, detector);
}

void loop() {
    lifecycle.update();
    handleRunning();
}
