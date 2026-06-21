---
sidebar_position: 4
---

# Hardware

## MCU: ESP32-C3

- 400 KB SRAM (16 KB for cache)
- 4 MB flash
- WiFi 802.11 b/g/n + BLE 5.0
- RISC-V single core, 160 MHz
- Deep sleep: ~5 µA

Development board: **ESP32-C3-DevKitC-02**

## LED Layout

```
┌─────────────────┐
│   MAIN SIGNAL   │
│                 │
│   🔴 Red LED    │   ← single color LED
│   🟢 Green LED  │   ← single color LED
│                 │
├─────────────────┤
│   PRE-SIGNAL    │
│                 │
│   ⚪ LED 1      │   ← color TBD (RGB or single-color)
│   ⚪ LED 2      │   ← color TBD (RGB or single-color)
│                 │
└─────────────────┘
```

### Main Signal
- 1× Red LED
- 1× Green LED
- Only one lit at a time

### Pre-Signal
- 2× LEDs showing either YELLOW or GREEN
- **Option A:** 2× RGB LEDs (e.g., WS2812B addressable) — 1 data pin, full color flexibility
- **Option B:** 2× yellow + 2× green single-color LEDs — 4 pins, simpler circuit

:::note Decision Pending
RGB LED type not yet decided. WS2812B are 5V — voltage compatibility with ESP32-C3 (3.3V) needs checking. Alternatives: SK6812 (works at 3.3V-5V), or separate single-color LEDs.
:::

## Sensors

### Train Detection: IR Break-Beam

- 1× IR LED (transmitter) — one side of track
- 1× Phototransistor (receiver) — other side of track
- Train passes between them → beam broken → detection

| Property | Value |
|----------|-------|
| Cost | ~€0.50 per pair |
| Power | ~1 mA (can pulse to save power) |
| Reliability | Very high — physical interruption, no false triggers |
| Mounting | Across the track rails |

## Buttons

- 2× tactile push buttons
- **Button 1 (Green):** Set signal to GREEN / pairing role
- **Button 2 (Red):** Set signal to RED / pairing role

## GPIO Assignment (Preliminary)

| GPIO | Function | Notes |
|------|----------|-------|
| TBD | Red LED (main) | Output, active high |
| TBD | Green LED (main) | Output, active high |
| TBD | Pre-signal data / LEDs | 1 pin if WS2812B, more if single-color |
| TBD | IR sensor input | Input, pulled up, goes low on detection |
| TBD | Button 1 (green) | Input, internal pull-up, active low |
| TBD | Button 2 (red) | Input, internal pull-up, active low |

:::note
GPIO pin assignments will be defined once the physical PCB/wiring layout is decided. Until then, all pin numbers are abstracted behind defines in `include/config/pins.h`.
:::

## Power

- **Battery:** LiPo 3.7V, ~1000mAh
- **Charging:** TP4056 USB-C charger board
- **Regulation:** LDO or direct battery to ESP32-C3 (operates at 3.0-3.6V)

### Estimated Power Budget

| Component | Current |
|-----------|---------|
| ESP32-C3 active + ESP-NOW idle | ~20 mA |
| Main signal LED (1 lit) | ~5-10 mA |
| Pre-signal LEDs (2 lit) | ~10-20 mA |
| IR sensor | ~1 mA |
| **Total during play** | **~35-50 mA** |
| **Battery life (1000mAh)** | **~20-28 hours** |

Deep sleep standby: ~5 µA → effectively years.
