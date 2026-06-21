---
sidebar_position: 2
---

# Architecture

## Overview

Each signal mast runs identical firmware. Behavior is determined solely by configuration:
- `my_id` — this signal's number (used for identification at boot)
- `next_id` — the signal whose main state this signal shows on its pre-signal
- `peer_mac` — MAC address of the next signal (learned during pairing)

## Topology

Signals can be arranged in a **line** or a **loop**:

### Line (4 signals)

```
Signal 1 ──▶ Signal 2 ──▶ Signal 3 ──▶ Signal 4
```

Signal 4 has no next signal — its pre-signal is always green (or off).

### Loop (4 signals)

```
Signal 1 ──▶ Signal 2
   ▲              │
   │              ▼
Signal 4 ◀── Signal 3
```

Signal 4's next signal is Signal 1. Every signal behaves identically.

## State Machine

Each signal's main signal has two states:

```
        ┌──────────────────┐
        │                  │
        ▼                  │
   ┌─────────┐       ┌─────────┐
   │  GREEN  │──────▶│   RED   │
   └─────────┘       └─────────┘
        ▲                  │
        │                  │
        └──────────────────┘

Triggers:
  GREEN → RED:  Train detected (IR sensor) or red button pressed
  RED → GREEN:  Timer expires or green button pressed
```

## Data Flow

```
Signal 2 main → RED
       │
       │ ESP-NOW broadcast
       ▼
Signal 1 receives → pre-signal shows YELLOW

Signal 2 main → GREEN
       │
       │ ESP-NOW broadcast
       ▼
Signal 1 receives → pre-signal shows GREEN
```

## Software Components

```
src/
├── main.cpp              # Entry point, lifecycle kick-off
├── lifecycle/
│   └── LifecycleManager  # Boot stage sequencing
├── signal/
│   └── SignalController   # State machine, LED control
├── comm/
│   └── EspNowManager     # ESP-NOW send/receive/pairing
├── input/
│   └── ButtonHandler     # Button debounce, press/hold detection
└── sensor/
    └── TrainDetector     # IR break-beam logic
```

## Design Principles

1. **Battery first** — every decision considers power consumption
2. **No external dependencies** — no router, server, or phone needed during play
3. **Identical firmware** — all signals run the same code, configured per device
4. **Fail safe** — if communication fails, signal keeps last known state
5. **Kid-proof** — simple buttons, automatic operation, visual feedback
