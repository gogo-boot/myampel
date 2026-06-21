---
sidebar_position: 3
---

# Communication (ESP-NOW)

## Why ESP-NOW

| Requirement | ESP-NOW | MQTT over WiFi |
|-------------|---------|----------------|
| No extra hardware | ✅ Peer-to-peer | ❌ Needs router + broker |
| Low power | ✅ ~20 mA idle | ❌ ~80-100 mA |
| Low latency | ✅ ~5 ms | ~50-100 ms |
| Battery life (1000mAh) | ~40-50 hours | ~10-12 hours |

ESP-NOW is a connectionless protocol by Espressif. It uses the WiFi radio but without TCP/IP overhead — like UDP over the air. No handshake, no association, no router.

## Trade-offs

ESP-NOW has **no message persistence**. If a signal is off when a message is sent, that message is lost forever. MQTT brokers retain messages — ESP-NOW does not.

We compensate with a [reliability layer](#reliability-layer).

## Message Format

Each signal broadcasts a simple state message:

```c
struct SignalMessage {
    uint8_t signal_id;      // Sender's ID
    uint8_t main_state;     // RED=0, GREEN=1
    uint8_t msg_type;       // STATE=0, PAIR_OFFER=1, PAIR_ACCEPT=2
    uint8_t sequence;       // For duplicate detection
};
```

Total: 4 bytes per message. Minimal air time.

## Reliability Layer

Since ESP-NOW can lose messages, we add two mechanisms:

### 1. Heartbeat Broadcast (every 2 seconds)

Every signal broadcasts its current main state every 2 seconds — even if nothing changed. If a message is missed, the next heartbeat corrects it.

**Worst case**: A signal shows wrong pre-signal state for up to 2 seconds.

### 2. ACK + Retry (on state change)

When a signal's main state changes (the important event):
1. Send state change message
2. Wait for ACK (ESP-NOW provides frame-level ACK)
3. If no ACK: retry up to 3 times with 50ms delay

### 3. State Request on Boot

When a signal boots up, it doesn't know the current state of its next signal. It broadcasts a state request, and peers respond with their current state.

If no response within 2 seconds: assume GREEN (safe default — pre-signal shows green).

## Timing

| Event | Timing |
|-------|--------|
| State change delivery | ~5 ms (immediate) |
| Heartbeat interval | 2000 ms |
| ACK timeout | 50 ms |
| Max retries | 3 |
| Boot state request timeout | 2000 ms |

## ESP-NOW Limitations on ESP32-C3

- Max **6 encrypted peers** (enough for typical setups)
- Max **20 unencrypted peers**
- Payload max 250 bytes (we use 4 bytes)
- Range: ~50m indoor, ~200m open air
