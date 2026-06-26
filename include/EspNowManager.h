#pragma once
#include "signal_types.h"

class EspNowManager {
public:
    void begin(uint8_t signalId);
    void update();
    void sendState(MainSignal state);
    void requestState();

    // Pairing
    void enterPairing();
    void exitPairing();
    void sendPairOffer();
    void sendPairAccept(const uint8_t* mac);
    bool isPairing() const { return _pairing; }
    bool pairingTimedOut() const;
    bool hasPairOffer() const { return _hasPairOffer; }
    void acceptPairOffer();
    void getPairOfferMac(uint8_t mac[6]) const;

    // Reception
    bool hasPeerState() const { return _hasPeerState; }
    bool received() const { return _received; }
    MainSignal peerMainState() const { return _peerMain; }
    bool peerDetectedTrain() const { return _peerDetectedTrain; }

    // Peer management
    void clearPeers();
    void addPeer(const uint8_t mac[6]);
    void loadPeersFromNvs();
    void savePeersToNvs();
    uint8_t peerCount() const { return _peerCount; }

private:
    static void onReceive(const uint8_t* mac, const uint8_t* data, int len);
    void broadcast(const SignalMessage& msg);

    static EspNowManager* _instance;
    uint8_t _signalId = 1;
    uint8_t _sequence = 0;
    uint32_t _lastHeartbeat = 0;

    // Peers
    uint8_t _peers[MAX_PEERS][6] = {};
    uint8_t _peerCount = 0;

    // Pairing state
    bool _pairing = false;
    uint32_t _pairingStart = 0;
    bool _hasPairOffer = false;
    uint8_t _pairOfferMac[6] = {};

    // Received state
    bool _hasPeerState = false;
    bool _received = false;
    bool _peerDetectedTrain = false;
    MainSignal _peerMain = MainSignal::GREEN;
    uint8_t _lastSequence[MAX_PEERS] = {};
};
