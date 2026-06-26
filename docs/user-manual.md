---
sidebar_position: 12
---

# User Manual

## What's in the Box

Each MyAmpel signal mast has:
- A signal pole with colored lights
- Two buttons (🟢 Green and 🔴 Red)
- A USB-C port for charging

## Charging

1. Plug a USB-C cable into the port on the base
2. Red LED on the board = charging
3. Green LED on the board = fully charged
4. Full charge takes ~1–5 hours (depending on battery size)

You can play while charging.

## Turning On / Off

- **On:** Press any button — the signal wakes up and starts
- **Off:** The signal automatically sleeps after 5 minutes of no activity (no trains, no button presses)
- All lights turn off when sleeping

## Signal Lights Explained

### Main Signal (top, rectangle)

| Light | Meaning |
|-------|---------|
| 🔴 Red | Stop — a train just passed |
| 🟢 Green | Go — track is clear |

### Pre-Signal (bottom, diamond shape)

| Light | Meaning |
|-------|---------|
| 🟡🟡 Yellow pair | Warning — the NEXT signal ahead is red |
| 🟢🟢 Green pair | All clear — the NEXT signal ahead is green |

The pre-signal updates automatically when it hears from the next signal.

## Buttons

### During Play

| Button | What it does |
|--------|-------------|
| 🟢 Green button (short press) | Force this signal to GREEN |
| 🔴 Red button (short press) | Force this signal to RED |

When you override a signal manually:
- The main signal changes immediately (red or green)
- The **previous signal's pre-signal** also updates automatically (yellow or green) — because it always shows what this signal is showing

### Pairing Mode

| Action | What it does |
|--------|-------------|
| Hold **both buttons** for 3 seconds | Enter pairing mode (all stored peers are cleared, lights blink fast) |
| In pairing mode: press 🟢 Green | "I am the next signal" — offers itself to a partner |
| In pairing mode: press 🔴 Red | "I accept" — stores the partner as the next signal |

Each successful pairing extends the 30-second timeout so you have time to prepare the next signal.

**Example:** To pair Signal 1 → Signal 2:
1. Both signals: hold both buttons 3s → lights blink fast
2. Signal 2: press green ("I am the next signal")
3. Signal 1: press red ("I accept you as my next")
4. Both flash green to confirm → done

**Station mode (multiple platforms):** Repeat pairing on the same signal to add more peers. A signal with 2+ peers automatically becomes a station entry signal.

Pairing tells each signal who its neighbor is. See [Pairing](./pairing) for step-by-step setup.

## Setting Up on the Track

1. Place signals along the track — at stations, junctions, or anywhere you like
2. Make sure the IR sensor on the base points toward the track (~5 cm from the rails)
3. Pair the signals in order (see [Pairing](./pairing))
4. Press the green button on each signal to start with all green
5. Play! Trains will trigger signals automatically

## How It Works (Simple Version)

```
🚂 Train passes Signal 2
  → Signal 2 turns RED (stop for next train)
  → Signal 1's pre-signal turns YELLOW (warning: next is red!)

⏱️ After 30 seconds...
  → Signal 2 turns GREEN again (track clear)
  → Signal 1's pre-signal turns GREEN (all clear ahead)
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Signal won't turn on | Charge it via USB-C |
| Signal doesn't detect trains | Move closer to the track (within ~5 cm) |
| Pre-signal stuck on yellow/green | Check pairing — re-pair the signals |
| Lights all blink fast | In pairing mode — press a button or wait 30s to exit |
| Signal turns off during play | Battery empty — charge it |
