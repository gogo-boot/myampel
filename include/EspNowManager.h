#pragma once
#include "signal_types.h"
#include <cstdint>

class EspNowManager {
public:
    void begin();
    void update();
    void setPeer(const uint8_t mac[6]);
    void requestState();
    void sendState(uint8_t signalId, MainSignal state);
    bool hasPeerState() const { return _hasPeerState; }
    bool received() const { return _received; }

    // Get received data
    MainSignal peerMainState() const { return _peerMain; }
    bool isPairOffer() const { return _isPairOffer; }
    void getPeerMac(uint8_t mac[6]) const;

    // Pairing
    void sendPairOffer(uint8_t signalId);
    void sendPairAccept(uint8_t signalId);

private:
    static void onReceive(const uint8_t* mac, const uint8_t* data, int len);
    void send(const SignalMessage& msg);

    uint8_t _peerMac[6] = {};
    bool _peerRegistered = false;
    bool _hasPeerState = false;
    bool _received = false;
    bool _isPairOffer = false;
    MainSignal _peerMain = MainSignal::GREEN;
    uint8_t _sequence = 0;
    uint32_t _lastHeartbeat = 0;

    // Static instance pointer for callback
    static EspNowManager* _instance;
};
