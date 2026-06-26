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

// === Pairing Mode Logic ===
static bool inPairing = false;

static void handlePairing() {
    if (!inPairing) return;

    comm.update();
    buttons.update();

    // Green button → offer self as "next signal"
    if (buttons.greenEvent() == ButtonEvent::SHORT_PRESS) {
        comm.sendPairOffer();
    }

    // Red button → accept incoming offer
    if (buttons.redEvent() == ButtonEvent::SHORT_PRESS && comm.hasPairOffer()) {
        comm.acceptPairOffer();
        // Flash green to confirm
        signalCtrl.identifyFlash();
    }

    // Timeout → exit
    if (comm.pairingTimedOut()) {
        comm.exitPairing();
        inPairing = false;
    }
}

// === Running Mode Logic ===
static void handleRunning() {
    if (lifecycle.stage() != Stage::RUNNING) return;

    // Check for pairing mode entry (both buttons held 3s)
    if (buttons.bothHeldTriggered()) {
        comm.enterPairing();
        inPairing = true;
        signalCtrl.allOff();
        return;
    }

    if (inPairing) {
        handlePairing();
        return;
    }

    // Train detection → RED
    if (detector.triggered()) {
        signalCtrl.setMain(MainSignal::RED);
    }

    // Green button → force GREEN
    if (buttons.greenEvent() == ButtonEvent::SHORT_PRESS) {
        signalCtrl.setMain(MainSignal::GREEN);
    }

    // Red button → force RED
    if (buttons.redEvent() == ButtonEvent::SHORT_PRESS) {
        signalCtrl.setMain(MainSignal::RED);
    }

    // Next signal detected train → our block is clear
    if (comm.peerDetectedTrain()) {
        signalCtrl.notifyBlockClear();
    }

    // Receive peer state → update pre-signal
    if (comm.received()) {
        PreSignal pre = (comm.peerMainState() == MainSignal::RED)
            ? PreSignal::YELLOW : PreSignal::GREEN;
        signalCtrl.setPre(pre);
    }

    // Broadcast on state change
    if (signalCtrl.mainChanged()) {
        comm.sendState(signalCtrl.mainState());
        signalCtrl.clearMainChanged();
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
