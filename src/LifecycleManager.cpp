#include "LifecycleManager.h"
#include "SignalController.h"
#include "ButtonHandler.h"
#include "EspNowManager.h"
#include "TrainDetector.h"
#include "config/pins.h"
#include <Arduino.h>
#include <Preferences.h>

static const char* TAG = "Lifecycle";

void LifecycleManager::begin(SignalController& signal, ButtonHandler& buttons,
                             EspNowManager& comm, TrainDetector& detector) {
    _signal = &signal;
    _buttons = &buttons;
    _comm = &comm;
    _detector = &detector;
    _lastActivity = millis();
    enterBoot();
}

void LifecycleManager::update() {
    uint32_t now = millis();

    switch (_stage) {
        case Stage::BOOT:
            if (now - _stageStart >= SELFTEST_DURATION_MS) enterConfig();
            break;

        case Stage::CONFIG:
            enterIdentify();
            break;

        case Stage::IDENTIFY:
            if (now - _stageStart >= IDENTIFY_DURATION_MS) enterConnect();
            break;

        case Stage::CONNECT:
            if (_comm->hasPeerState() || now - _stageStart >= CONNECT_TIMEOUT_MS)
                enterRunning();
            break;

        case Stage::RUNNING:
            _buttons->update();
            _detector->update();
            _comm->update();
            _signal->update();

            if (_buttons->greenEvent() != ButtonEvent::NONE ||
                _buttons->redEvent() != ButtonEvent::NONE ||
                _detector->triggered() || _comm->received()) {
                _lastActivity = now;
            }
            if (now - _lastActivity >= SLEEP_TIMEOUT_MS) enterSleep();
            break;

        case Stage::SLEEP:
            break;
    }
}

void LifecycleManager::enterBoot() {
    _stage = Stage::BOOT;
    _stageStart = millis();
    ESP_LOGI(TAG, "BOOT");
    _signal->begin();
    _buttons->begin();
    _signal->selfTest();
}

void LifecycleManager::enterConfig() {
    _stage = Stage::CONFIG;
    _stageStart = millis();
    ESP_LOGI(TAG, "CONFIG");

    Preferences prefs;
    prefs.begin("myampel", true);
    _signalId = prefs.getUChar("signal_id", 1);
    prefs.end();

    _signal->setSignalId(_signalId);
    _comm->loadPeersFromNvs();
    _signal->allOff();
}

void LifecycleManager::enterIdentify() {
    _stage = Stage::IDENTIFY;
    _stageStart = millis();
    ESP_LOGI(TAG, "IDENTIFY");
    _signal->identifyFlash();
}

void LifecycleManager::enterConnect() {
    _stage = Stage::CONNECT;
    _stageStart = millis();
    ESP_LOGI(TAG, "CONNECT");
    _comm->begin(_signalId);
    _detector->begin(_signalId);
    _comm->requestState();
}

void LifecycleManager::enterRunning() {
    _stage = Stage::RUNNING;
    _stageStart = millis();
    _lastActivity = millis();
    ESP_LOGI(TAG, "RUNNING");

    if (!_comm->hasPeerState()) {
        _signal->setMain(MainSignal::GREEN);
        _signal->setPre(PreSignal::GREEN);
    }
    _signal->clearMainChanged();
}

void LifecycleManager::enterSleep() {
    _stage = Stage::SLEEP;
    ESP_LOGI(TAG, "SLEEP");
    _signal->allOff();
    esp_deep_sleep_enable_gpio_wakeup(
        (1ULL << PIN_BTN_GREEN) | (1ULL << PIN_BTN_RED),
        ESP_GPIO_WAKEUP_GPIO_LOW);
    esp_deep_sleep_start();
}
