---
sidebar_position: 7
---

# Signal Logic

## German Railway Signal Basics

MyAmpel replicates simplified Deutsche Bahn signal rules:

- **Hauptsignal (Hp)**: Shows RED (Hp0 = stop) or GREEN (Hp1 = proceed)
- **Vorsignal (Vr)**: Shows the state of the NEXT Hauptsignal ahead
  - Next is RED → Vorsignal shows YELLOW (Vr0 = expect stop)
  - Next is GREEN → Vorsignal shows GREEN (Vr1 = expect proceed)

## State Table

| This signal's main | Meaning |
|-------|---------|
| 🔴 RED | Train must stop here |
| 🟢 GREEN | Train may proceed |

| This signal's pre-signal | Meaning | Cause |
|-------|---------|-------|
| 🟡 YELLOW | Next signal is RED — slow down | Next signal main = RED |
| 🟢 GREEN | Next signal is GREEN — full speed | Next signal main = GREEN |

## State Transitions

### Main Signal

```
         Train detected (IR sensor)
              or red button
GREEN ─────────────────────────────▶ RED
  ▲                                   │
  │     Timer expires (30s default)   │
  │         or green button           │
  └───────────────────────────────────┘
```

### Pre-Signal

Pre-signal is **purely reactive** — it mirrors the next signal's main state:

```
Receive: next signal = GREEN  →  pre-signal = GREEN
Receive: next signal = RED    →  pre-signal = YELLOW
```

No timer, no button, no local logic. Just reflects what it hears from ESP-NOW.

## Train Detection Sequence

When a train passes Signal 2:

```
Time 0.000s  IR sensor triggered on Signal 2
Time 0.001s  Signal 2 main → RED
Time 0.005s  ESP-NOW broadcast: "Signal 2 is RED"
Time 0.005s  Signal 1 receives → pre-signal → YELLOW
Time 30.00s  Signal 2 timer expires → main → GREEN
Time 30.00s  ESP-NOW broadcast: "Signal 2 is GREEN"
Time 30.00s  Signal 1 receives → pre-signal → GREEN
```

## Manual Override

| Button | Action |
|--------|--------|
| Green button (short press) | Force main signal → GREEN, broadcast |
| Red button (short press) | Force main signal → RED, broadcast |

Manual override resets the auto-green timer.

## Default State

On boot (no peer info yet): **Main = GREEN, Pre = GREEN**

Rationale: GREEN is the safe default for a toy. The signal turns RED only when a train is detected or a button is pressed. This means play starts immediately without needing all signals to "negotiate."

## Timing

| Parameter | Default | Notes |
|-----------|---------|-------|
| Auto-green timer | 30 seconds | After RED, automatically go GREEN |
| Heartbeat interval | 2 seconds | Periodic state broadcast |
| IR debounce | 100 ms | Prevent false triggers |
| Button debounce | 50 ms | Standard debounce |

:::note
The auto-green timer (30s) is a default. It could be configurable via button combo or NVS. For now, hardcoded is fine.
:::
