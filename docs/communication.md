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

### Why `sequence`?

Duplicate detection. ESP-NOW can deliver the same message twice (sender retry + original both arrive). The receiver checks: "Did I already process sequence #42 from signal #3?" If yes, ignore. Without this, a single train passing could trigger two RED events.

### Why `uint8_t` instead of `bool` for `main_state`?

- **Extensibility** — if a YELLOW or OFF state is added later, bool can't represent it. uint8_t gives 256 possible values at no extra cost.
- **Predictable size** — `bool` size is compiler-dependent (1 or 4 bytes). Using `uint8_t` guarantees the struct is exactly 4 bytes with a known layout — important when sending raw bytes over the air.

## Reliability Layer

Since ESP-NOW can lose messages, we add two mechanisms:

### 1. Heartbeat Broadcast (every 2 seconds)

Every signal broadcasts its current main state every 2 seconds — even if nothing changed. If a message is missed, the next heartbeat corrects it.

**Worst case**: A signal shows wrong pre-signal state for up to 2 seconds.

**Why 2 seconds?** A balance between responsiveness and efficiency:

- State changes are delivered instantly (~5ms) via ACK+retry — the heartbeat is only a backup if that message was lost
- A kid won't notice a 2s delay on a pre-signal correction
- Low radio traffic (1800 messages/hour per signal — trivial for ESP-NOW)
- At 2s, duty cycle is 0.8% — still legal on 868 MHz if migrating to LoRa later
- Shorter (500ms) wastes battery; longer (10s) feels "broken" to the user

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

:::note
The peer limit only applies to **unicast** (sending to a specific MAC). **Broadcast** (to `FF:FF:FF:FF:FF:FF`) has no limit — unlimited signals can receive a broadcast. MyAmpel uses broadcast for heartbeats, so station mode with many platform signals works without hitting the 20-peer limit.
:::

## Station Mode Communication

In station mode, signals use **broadcast** (no peer limit) so all platform signals and entry signals communicate freely.

- Each platform signal **broadcasts** its main state (same as normal)
- Each platform signal **listens to** its next signal (Signal 3) for pre-signal
- Signal 1 (station entry) **listens to all platform signals** and shows pre-signal of the active route

No special messages needed — the same `SignalMessage` struct works. Signal 1 just filters by multiple known peer IDs instead of one.

## Future Option: Sub-GHz LoRa (SX1262)

ESP-NOW is the current implementation. For longer range and better battery life, a sub-GHz LoRa radio (SX1262) is a possible future upgrade. See [Radio & Frequency Options](./radio) for the full comparison.

### Summary

| | ESP-NOW (current) | LoRa SX1262 (future) |
|---|---|---|
| Frequency | 2.4 GHz | 433 or 868 MHz |
| Range | ~50m indoor, ~200m open | ~300-500m+ |
| RX idle current | 20 mA | 4.5 mA |
| Latency | ~5 ms | ~20-50 ms |
| Extra hardware | None | SX1262 module (~€3) |
| Duty cycle | No limit | 433: ≤10%, 868: ≤1% |
| Wall penetration | Worst | Best (433 MHz) |

### Duty Cycle

In Europe, sub-GHz bands have legal limits on how long you may transmit per hour:

- **868 MHz**: Max 1% → 36 seconds of TX per hour
- **433 MHz**: Max 10% → 360 seconds of TX per hour
- **2.4 GHz (ESP-NOW)**: No limit

With a 5-second heartbeat and ~15ms per message, MyAmpel uses ~0.3% duty cycle — legal on both bands. A 2-second heartbeat (like the current ESP-NOW implementation) would use ~0.8% — still legal on 868 MHz but tight. The 433 MHz band has plenty of headroom.

### Migration Path

The firmware architecture is decoupled from the radio layer. Swapping ESP-NOW for LoRa requires replacing `EspNowManager` with a `LoRaManager` using the RadioLib library — no changes to signal logic, LEDs, or buttons.

```ini
# platformio.ini addition for LoRa
lib_deps =
    jgromes/RadioLib@^7.6.0
```

### Why Not LoRa Now?

- ESP-NOW needs no extra hardware — simplest first implementation
- Range (~50m) is more than enough for a toy in the same room
- LoRa adds cost (+€3/signal), wiring (7 SPI pins), and a separate antenna
- Can be added later without changing the rest of the firmware
