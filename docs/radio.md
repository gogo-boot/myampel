---
sidebar_position: 8
---

# Radio & Frequency Options

## Overview

MyAmpel signals communicate wirelessly. In Europe, three license-free frequency bands are available without extra certification (when using pre-certified modules):

| Band | Frequency | Max Power (EU) | Duty Cycle | Antenna Length |
|------|-----------|---------------|------------|----------------|
| **433 MHz** | 433.05–434.79 MHz | 10 mW | ≤10% | 17.3 cm |
| **868 MHz** | 868.0–868.6 MHz | 25 mW | ≤1% | 8.6 cm |
| **2.4 GHz** | 2400–2483 MHz | 100 mW (WiFi) | No limit | 3.1 cm (PCB trace) |

## Duty Cycle Explained

Duty cycle = how much time your radio may transmit per hour.

- **868 MHz at 1%**: Max 36 seconds of transmission per hour
- **433 MHz at 10%**: Max 360 seconds per hour
- **2.4 GHz**: No legal limit

### MyAmpel Duty Cycle Calculation

With a 5-second heartbeat interval and ~15ms per message (LoRa SF7, 4 bytes):

```
Messages per hour: 720
TX time per hour:  720 × 15ms = 10.8 seconds = 0.3%

868 MHz (1% limit): ✅ Legal
433 MHz (10% limit): ✅ Very legal
```

## Protocol Options

### Option A: ESP-NOW (2.4 GHz) — Current Implementation

ESP-NOW is a connectionless WiFi-based protocol by Espressif. No router or server needed.

| Property | Value |
|----------|-------|
| Range | ~50–200m |
| Latency | ~5 ms |
| RX idle current | ~20 mA |
| Extra hardware | None (built into ESP32-C3) |
| Module cost | €0 |
| Antenna | Built into module (PCB trace) |

**Pros:**
- No extra hardware — simplest solution
- No duty cycle limit
- Lowest latency
- Already implemented in firmware

**Cons:**
- Highest power consumption while listening
- Shorter range
- Worst wall penetration of the three bands

### Option B: Sub-GHz LoRa (433/868 MHz)

Uses a separate radio module (SX1262) connected via SPI.

| Property | 433 MHz | 868 MHz |
|----------|---------|---------|
| Range (same power) | Better (longer wavelength) | Good |
| Range (max legal power) | ~300–500m | ~500m+ |
| Wall penetration | Best | Good |
| Latency | ~20–50 ms | ~20–50 ms |
| RX idle current | 4.5 mA | 4.5 mA |
| Duty cycle | 10% (relaxed) | 1% (tight) |
| Antenna | 17.3 cm wire | 8.6 cm wire |
| Extra hardware | SX1262 module (~€3) | SX1262 module (~€3) |

**Pros:**
- 4× lower RX power → ~2× battery life
- Much longer range
- Better obstacle penetration

**Cons:**
- Extra module (cost, PCB space, wiring)
- Higher latency (~20–50ms vs 5ms)
- Needs external antenna
- More GPIO pins required (7 total for SPI + control)

:::info Important: SX1262 is a separate radio module
The SX1262 is **not a microcontroller** — it has no CPU and runs no code. It is a radio peripheral that your ESP32 controls via SPI commands. Think of it like an LED or sensor: you wire it to your ESP32 and send commands from your firmware.

This means sub-GHz adds cost and complexity:
- You still need an ESP32-C3 (~€1.50) as the brain
- Plus the SX1262 module (~€3) as the radio
- Plus an antenna matched to your frequency
- Plus 7 GPIO pins for the SPI connection

**Combined modules exist** (ESP32 + SX1262 on one board, e.g., Heltec HT-CT62) but cost ~€8–15 — significantly more than a standalone ESP32-C3 (~€1.50).
:::

## Range Comparison

At **equal transmit power**, lower frequency = longer range because:
1. Less free-space path loss
2. Better diffraction around obstacles
3. Better penetration through walls

However, EU allows 2.5× more power on 868 MHz (25 mW vs 10 mW), so 868 MHz achieves longer range in practice despite the higher frequency.

**For a toy train in the same room** (~5–10m), all three bands are massive overkill.

## Power Comparison

The signal listens continuously, so **RX idle current** dominates battery life:

| Solution | RX Idle | LEDs | ESP32 | Total | Battery Life (1000mAh) |
|----------|---------|------|-------|-------|----------------------|
| ESP-NOW (2.4 GHz) | 20 mA | 10 mA | 15 mA | ~45 mA | **~22 hours** |
| SX1262 (sub-GHz) | 4.5 mA | 10 mA | 10 mA | ~25 mA | **~40 hours** |

With sub-GHz, the ESP32's WiFi radio stays off entirely → CPU draws less too.

## SX1262 Module Comparison

| Chip | Generation | Frequency | RX Current | Pins Needed | Cost |
|------|-----------|-----------|------------|-------------|------|
| SX1276 | 1st gen | 868 MHz only | 10.3 mA | 6 | ~€2 |
| SX1278 | 1st gen | 433 MHz only | 10.3 mA | 6 | ~€2 |
| **SX1262** | **2nd gen** | **150–960 MHz (both)** | **4.5 mA** | **7** | **~€3** |

**SX1262 is recommended** — halved RX current outweighs the extra €1 and 1 extra pin.

:::note
The SX1262 chip supports many frequencies, but the **antenna must match your chosen frequency**. Buy a module labeled for your target band (e.g., "SX1262 433MHz" comes with a 433 MHz antenna).
:::

## Antenna Considerations

| Band | ¼ Wave Antenna | Fits in Signal Mast (~18cm)? | Antenna Options |
|------|---------------|------------------------------|-----------------|
| 433 MHz | 17.3 cm | Tight — can run wire along inside the pole | Wire, spring coil |
| 868 MHz | 8.6 cm | ✅ Easily fits | Wire, spring coil, ceramic chip |
| 2.4 GHz | 3.1 cm | ✅ On-chip (no extra antenna) | PCB trace (built-in) |

The 433 MHz antenna length (~17cm) happens to match a typical signal mast height — the antenna wire can double as structural support or hide inside the pole.

## Current Decision

| Aspect | Decision | Rationale |
|--------|----------|-----------|
| Protocol | ESP-NOW (2.4 GHz) | Simplicity — no extra hardware, already working |
| Future option | SX1262 at 433 MHz | Best battery life, relaxed duty cycle, fits in mast |

:::tip Future Migration Path
The firmware architecture (LifecycleManager + SignalController) is decoupled from the radio layer. Swapping ESP-NOW for SX1262 only requires replacing `EspNowManager` with a `LoRaManager` — no changes to signal logic, LEDs, or buttons.
:::

## Available Modules (for future sub-GHz implementation)

| Module | Chip | Frequency | Interface | Price | Notes |
|--------|------|-----------|-----------|-------|-------|
| Ebyte E22-400M | SX1262 | 433 MHz | SPI | ~€3 | Small, good for PCB |
| Ebyte E22-900M | SX1262 | 868 MHz | SPI | ~€3 | Same module, 868 band |
| Waveshare SX1262 | SX1262 | 868 MHz | SPI | ~€5 | Breakout for prototyping |
| Ra-01 (Ai-Thinker) | SX1278 | 433 MHz | SPI | ~€2 | Older chip, higher RX power |

## Programming

All modules work with PlatformIO + Arduino framework using the **RadioLib** library:

```ini
# platformio.ini
lib_deps =
    jgromes/RadioLib@^7.6.0
```

RadioLib provides a unified API for SX1262, SX1276, SX1278, and many other radio chips.
