#include "EspNowManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

static const char* TAG = "EspNow";

EspNowManager* EspNowManager::_instance = nullptr;

void EspNowManager::begin() {
    _instance = this;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW init failed");
        return;
    }
    esp_now_register_recv_cb(onReceive);

    if (_peerRegistered) {
        esp_now_peer_info_t peer = {};
        memcpy(peer.peer_addr, _peerMac, 6);
        peer.channel = 0;
        peer.encrypt = false;
        esp_now_add_peer(&peer);
    }
    ESP_LOGI(TAG, "ESP-NOW initialized");
}

void EspNowManager::update() {
    _received = false;
    _isPairOffer = false;
}

void EspNowManager::setPeer(const uint8_t mac[6]) {
    memcpy(_peerMac, mac, 6);
    _peerRegistered = true;
}

void EspNowManager::requestState() {
    SignalMessage msg = {0, 0, (uint8_t)MsgType::STATE_REQUEST, _sequence++};
    // Broadcast to all
    uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_peer_info_t bcast = {};
    memcpy(bcast.peer_addr, broadcast, 6);
    bcast.channel = 0;
    bcast.encrypt = false;
    esp_now_add_peer(&bcast);
    esp_now_send(broadcast, (uint8_t*)&msg, sizeof(msg));
    esp_now_del_peer(broadcast);
}

void EspNowManager::sendState(uint8_t signalId, MainSignal state) {
    SignalMessage msg = {signalId, (uint8_t)state, (uint8_t)MsgType::STATE, _sequence++};
    send(msg);
    _lastHeartbeat = millis();
}

void EspNowManager::sendPairOffer(uint8_t signalId) {
    SignalMessage msg = {signalId, 0, (uint8_t)MsgType::PAIR_OFFER, _sequence++};
    uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_peer_info_t bcast = {};
    memcpy(bcast.peer_addr, broadcast, 6);
    bcast.encrypt = false;
    esp_now_add_peer(&bcast);
    esp_now_send(broadcast, (uint8_t*)&msg, sizeof(msg));
    esp_now_del_peer(broadcast);
}

void EspNowManager::sendPairAccept(uint8_t signalId) {
    SignalMessage msg = {signalId, 0, (uint8_t)MsgType::PAIR_ACCEPT, _sequence++};
    send(msg);
}

void EspNowManager::send(const SignalMessage& msg) {
    if (!_peerRegistered) return;
    for (uint8_t attempt = 0; attempt <= ESP_NOW_MAX_RETRIES; attempt++) {
        esp_err_t result = esp_now_send(_peerMac, (uint8_t*)&msg, sizeof(msg));
        if (result == ESP_OK) return;
        delay(ESP_NOW_RETRY_DELAY_MS);
    }
    ESP_LOGW(TAG, "Send failed after retries");
}

void EspNowManager::getPeerMac(uint8_t mac[6]) const {
    memcpy(mac, _peerMac, 6);
}

void EspNowManager::onReceive(const uint8_t* mac, const uint8_t* data, int len) {
    if (!_instance || len != sizeof(SignalMessage)) return;

    SignalMessage msg;
    memcpy(&msg, data, sizeof(msg));

    MsgType type = (MsgType)msg.msg_type;

    if (type == MsgType::STATE) {
        _instance->_peerMain = (MainSignal)msg.main_state;
        _instance->_hasPeerState = true;
        _instance->_received = true;
    } else if (type == MsgType::PAIR_OFFER) {
        memcpy(_instance->_peerMac, mac, 6);
        _instance->_isPairOffer = true;
        _instance->_received = true;
    } else if (type == MsgType::STATE_REQUEST) {
        // Respond with current state if we have a signal controller
        _instance->_received = true;
    }
}
