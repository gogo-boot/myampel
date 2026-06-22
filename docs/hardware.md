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
│   🟡 Yellow 1   │   ← single color LED
│   🟡 Yellow 2   │   ← single color LED
│   🟢 Green 1    │   ← single color LED
│   🟢 Green 2    │   ← single color LED
│                 │
└─────────────────┘
```

### Main Signal
- 1× Red LED
- 1× Green LED
- Only one lit at a time

### Pre-Signal
- 2× Yellow LEDs — lit when next signal is RED (Vr0 = expect stop)
- 2× Green LEDs — lit when next signal is GREEN (Vr1 = expect proceed)
- Yellow pair OR green pair lit at a time, never both

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

## GPIO Assignment (ESP32-C3-MINI-1)

The ESP32-C3-MINI-1 exposes 15 GPIOs. After reserving USB (GPIO18/19) and UART0 (GPIO20/21), 11 pins remain. We use 9:

| GPIO | Function | Direction | Notes |
|------|----------|-----------|-------|
| GPIO0 | Red LED (main) | Output | Safe pin |
| GPIO1 | Green LED (main) | Output | Safe pin |
| GPIO3 | Pre-signal Yellow 1 | Output | Safe pin |
| GPIO10 | Pre-signal Yellow 2 | Output | Safe pin |
| GPIO4 | Pre-signal Green 1 | Output | JTAG pin, fine as output |
| GPIO5 | Pre-signal Green 2 | Output | JTAG pin, fine as output |
| GPIO6 | Button Green | Input | Internal pull-up, active low |
| GPIO7 | Button Red | Input | Internal pull-up, active low |
| GPIO8 | IR Sensor | Input | Strapping pin, OK as input |

**Spare pins:** GPIO2 (strapping — can drive IR LED to pulse), GPIO9 (boot mode — leave free or use as wake pin)

### Pin Warnings

| Pin | Concern | Mitigation |
|-----|---------|------------|
| GPIO2 | Strapping (boot mode) | Don't pull low at boot. Fine as output after boot. |
| GPIO4–7 | JTAG | Using these disables JTAG debugging. Acceptable for production. |
| GPIO8 | Strapping (rom_log) | OK as input — just don't pull low externally at boot. |
| GPIO9 | Strapping (boot_mode) | Left unconnected for clean boots. |

## Wiring

```
ESP32-C3-MINI-1
┌────────────────────────┐
│                        │
│  GPIO0  ──[220Ω]──────┤── 🔴 Red LED ────── GND
│  GPIO1  ──[220Ω]──────┤── 🟢 Green LED ──── GND
│                        │
│  GPIO3  ──[220Ω]──────┤── 🟡 Yellow LED 1 ─ GND
│  GPIO10 ──[220Ω]──────┤── 🟡 Yellow LED 2 ─ GND
│  GPIO4  ──[220Ω]──────┤── 🟢 Green LED 1 ── GND
│  GPIO5  ──[220Ω]──────┤── 🟢 Green LED 2 ── GND
│                        │
│  GPIO6  ──────────────┤── [BTN GREEN] ───── GND
│  GPIO7  ──────────────┤── [BTN RED] ─────── GND
│                        │   (internal pull-up)
│                        │
│  GPIO8  ──────────────┤── IR Phototransistor
│                        │   (pull-up, LOW on detect)
│                        │
│  3V3    ──────────────┤── IR LED ──[100Ω]── GND
│  GND    ──────────────┤── Common ground
└────────────────────────┘
```

### Component Notes

- **LEDs**: 220Ω resistor each (at 3.3V → ~5–10 mA per LED)
- **Buttons**: No external resistor — `INPUT_PULLUP` in firmware, button shorts pin to GND
- **IR Sensor**: Phototransistor with internal pull-up. Train breaks beam → pin goes LOW
- **IR LED**: Always-on from 3.3V rail (~1 mA). Optional: drive from GPIO2 to pulse and save power

Pin definitions are in `include/config/pins.h`.

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
