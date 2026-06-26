#include "EspNowManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Preferences.h>

static const char* TAG = "EspNow";
EspNowManager* EspNowManager::_instance = nullptr;
static const uint8_t BROADCAST_ADDR[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void EspNowManager::begin(uint8_t signalId) {
    _instance = this;
    _signalId = signalId;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "Init failed");
        return;
    }
    esp_now_register_recv_cb(onReceive);

    // Register broadcast peer
    esp_now_peer_info_t bcast = {};
    memcpy(bcast.peer_addr, BROADCAST_ADDR, 6);
    bcast.channel = 0;
    bcast.encrypt = false;
    esp_now_add_peer(&bcast);

    ESP_LOGI(TAG, "Initialized, id=%d, peers=%d", _signalId, _peerCount);
}

void EspNowManager::update() {
    _received = false;
    _peerDetectedTrain = false;
    _hasPairOffer = false;

    // Heartbeat
    if (!_pairing && (millis() - _lastHeartbeat >= HEARTBEAT_INTERVAL_MS)) {
        _lastHeartbeat = millis();
        SignalMessage msg = {_signalId, (uint8_t)_peerMain, (uint8_t)MsgType::STATE, _sequence};
        broadcast(msg);
    }
}

void EspNowManager::sendState(MainSignal state) {
    SignalMessage msg = {_signalId, (uint8_t)state, (uint8_t)MsgType::STATE, ++_sequence};
    // Broadcast immediately + retry
    for (uint8_t i = 0; i <= MAX_RETRIES; i++) {
        broadcast(msg);
        if (i < MAX_RETRIES) delay(RETRY_DELAY_MS);
    }
    _lastHeartbeat = millis();
}

void EspNowManager::requestState() {
    SignalMessage msg = {_signalId, 0, (uint8_t)MsgType::STATE_REQUEST, ++_sequence};
    broadcast(msg);
}

void EspNowManager::enterPairing() {
    _pairing = true;
    _pairingStart = millis();
    clearPeers();
    ESP_LOGI(TAG, "Pairing mode entered (peers cleared)");
}

void EspNowManager::exitPairing() {
    _pairing = false;
    savePeersToNvs();
    ESP_LOGI(TAG, "Pairing mode exited, %d peers stored", _peerCount);
}

bool EspNowManager::pairingTimedOut() const {
    return _pairing && (millis() - _pairingStart >= PAIRING_TIMEOUT_MS);
}

void EspNowManager::sendPairOffer() {
    SignalMessage msg = {_signalId, 0, (uint8_t)MsgType::PAIR_OFFER, ++_sequence};
    broadcast(msg);
    ESP_LOGI(TAG, "Pair offer sent");
}

void EspNowManager::sendPairAccept(const uint8_t* mac) {
    SignalMessage msg = {_signalId, 0, (uint8_t)MsgType::PAIR_ACCEPT, ++_sequence};
    broadcast(msg);
    ESP_LOGI(TAG, "Pair accept sent");
}

void EspNowManager::acceptPairOffer() {
    if (_hasPairOffer) {
        addPeer(_pairOfferMac);
        sendPairAccept(_pairOfferMac);
        _pairingStart = millis();  // Extend timeout
        ESP_LOGI(TAG, "Peer added, timeout extended");
    }
}

void EspNowManager::getPairOfferMac(uint8_t mac[6]) const {
    memcpy(mac, _pairOfferMac, 6);
}

void EspNowManager::clearPeers() {
    _peerCount = 0;
    memset(_peers, 0, sizeof(_peers));
}

void EspNowManager::addPeer(const uint8_t mac[6]) {
    if (_peerCount >= MAX_PEERS) return;
    memcpy(_peers[_peerCount], mac, 6);
    _peerCount++;
}

void EspNowManager::loadPeersFromNvs() {
    Preferences prefs;
    prefs.begin("myampel", true);
    _peerCount = prefs.getUChar("peer_count", 0);
    if (_peerCount > MAX_PEERS) _peerCount = 0;
    for (uint8_t i = 0; i < _peerCount; i++) {
        char key[12];
        snprintf(key, sizeof(key), "peer_%d", i);
        prefs.getBytes(key, _peers[i], 6);
    }
    prefs.end();
}

void EspNowManager::savePeersToNvs() {
    Preferences prefs;
    prefs.begin("myampel", false);
    prefs.putUChar("peer_count", _peerCount);
    for (uint8_t i = 0; i < _peerCount; i++) {
        char key[12];
        snprintf(key, sizeof(key), "peer_%d", i);
        prefs.putBytes(key, _peers[i], 6);
    }
    prefs.end();
}

void EspNowManager::broadcast(const SignalMessage& msg) {
    esp_now_send(BROADCAST_ADDR, (uint8_t*)&msg, sizeof(msg));
}

void EspNowManager::onReceive(const uint8_t* mac, const uint8_t* data, int len) {
    if (!_instance || len != sizeof(SignalMessage)) return;

    SignalMessage msg;
    memcpy(&msg, data, sizeof(msg));
    if (msg.signal_id == _instance->_signalId) return;  // Ignore own messages

    MsgType type = (MsgType)msg.msg_type;

    if (type == MsgType::STATE) {
        // Check if from a known peer
        for (uint8_t i = 0; i < _instance->_peerCount; i++) {
            if (memcmp(mac, _instance->_peers[i], 6) == 0) {
                // Duplicate detection
                if (msg.sequence == _instance->_lastSequence[i]) return;
                _instance->_lastSequence[i] = msg.sequence;

                MainSignal newState = (MainSignal)msg.main_state;
                // If peer just went RED, it detected a train → our block is clear
                if (newState == MainSignal::RED && _instance->_peerMain != MainSignal::RED) {
                    _instance->_peerDetectedTrain = true;
                }
                _instance->_peerMain = newState;
                _instance->_hasPeerState = true;
                _instance->_received = true;
                break;
            }
        }
    } else if (type == MsgType::PAIR_OFFER && _instance->_pairing) {
        memcpy(_instance->_pairOfferMac, mac, 6);
        _instance->_hasPairOffer = true;
        _instance->_received = true;
    } else if (type == MsgType::PAIR_ACCEPT && _instance->_pairing) {
        _instance->_received = true;
    } else if (type == MsgType::STATE_REQUEST) {
        // Respond with current state
        _instance->_received = true;
    }
}
