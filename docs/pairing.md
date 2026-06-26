---
sidebar_position: 6
---

# Pairing

## Overview

Pairing connects two signals so they know each other. Signal A needs to know: "Signal B is my next signal." This is done entirely with the two buttons on each device — no computer, phone, or app needed.

## Concept

Each signal only needs **one peer**: the next signal in the chain. Pairing stores the next signal's MAC address in NVS.

Pairing is **not a connection** — there is no handshake, no link, no registration between devices. It simply tells a signal "this is the MAC address you should listen for in broadcasts." The next signal doesn't know or care who is listening to it — it just broadcasts its state to everyone. The paired signal filters incoming broadcasts by MAC address and only reacts to its known peer.

## Pairing Flow

### Step 1: Enter Pairing Mode

**On both signals**: Hold both buttons (green + red) for 3 seconds.

→ LEDs blink rapidly to indicate pairing mode.

### Step 2: Signal B offers itself as "next signal"

**On Signal B** (the one that will be the next main signal): Press the **green button**.

→ Signal B broadcasts a `PAIR_OFFER` message containing its MAC address.
→ Signal B's green LED blinks slowly (waiting for acceptance).

### Step 3: Signal A accepts

**On Signal A** (the one that will show B's state on its pre-signal): Press the **red button**.

→ Signal A receives the `PAIR_OFFER`, stores Signal B's MAC address in NVS.
→ Signal A sends a `PAIR_ACCEPT` back to Signal B.
→ Both signals flash green to confirm success.
→ Both exit pairing mode.

## Example: Setting Up a 4-Signal Loop

```
Goal: 1→2→3→4→1

Pair 1: Signal 1 needs to know Signal 2
  - Both enter pairing mode (hold both buttons)
  - Signal 2 presses green ("I am the next signal")
  - Signal 1 presses red ("I accept you as my next")

Pair 2: Signal 2 needs to know Signal 3
  - Both enter pairing mode
  - Signal 3 presses green
  - Signal 2 presses red

Pair 3: Signal 3 needs to know Signal 4
  - Both enter pairing mode
  - Signal 4 presses green
  - Signal 3 presses red

Pair 4: Signal 4 needs to know Signal 1 (loop!)
  - Both enter pairing mode
  - Signal 1 presses green
  - Signal 4 presses red
```

## Signal ID Assignment

During pairing, the signal ID can be assigned by counting button presses:

- In pairing mode, after successful pairing, hold the **green button** and tap **red button** N times to set ID = N
- Or: assign at first flash (build-time config)

:::note
The signal ID is only used for the IDENTIFY blink at boot. Communication uses MAC addresses, not IDs.
:::

## Re-Pairing

To change a signal's peer:
1. Enter pairing mode (hold both buttons 3s) — **clears all stored peers**
2. Pair with the new signal(s)
3. Exit pairing mode (timeout or press both buttons again)

Entering pairing mode always starts fresh — old peers are erased. This prevents confusion between "replace" and "add."

## Timeout

If no pairing is completed within 30 seconds of entering pairing mode:
- Exit pairing mode
- Return to previous state (RUNNING or CONNECT)
- LEDs return to normal

**Timeout extension:** Each successful pairing resets the 30-second timeout. This gives time to prepare the next signal for station mode (pair multiple platforms without rushing).

## First Boot (Unpaired)

If a signal has never been paired (NVS empty):
- After BOOT and CONFIG stages detect empty NVS
- Automatically enter pairing mode
- Stay in pairing mode until paired or powered off

## Station Pairing (Multi-Platform)

Station mode uses the **same pairing flow** as normal mode — just repeated for each platform signal. The firmware auto-detects station mode when multiple next-peers are stored.

### Example: Signal 1 → 3 platforms → Signal 3

**Step 1–3: Pair Signal 1 to all platform signals**

| Step | Green button ("I am next") | Red button ("I accept") | Result |
|------|---|---|---|
| 1 | Signal 2.1 | Signal 1 | Signal 1 stores 2.1 as route option |
| 2 | Signal 2.2 | Signal 1 | Signal 1 adds 2.2 (now has 2 peers → station mode) |
| 3 | Signal 2.3 | Signal 1 | Signal 1 adds 2.3 (3 peers) |

**Step 4–6: Pair each platform signal to Signal 3**

| Step | Green button ("I am next") | Red button ("I accept") | Result |
|------|---|---|---|
| 4 | Signal 3 | Signal 2.1 | 2.1 stores Signal 3 as its next |
| 5 | Signal 3 | Signal 2.2 | 2.2 stores Signal 3 as its next |
| 6 | Signal 3 | Signal 2.3 | 2.3 stores Signal 3 as its next |

### How Station Mode Is Detected

No manual configuration needed. The firmware counts stored peers:

- **1 peer** → normal mode (line/loop)
- **2+ peers** → station entry mode (listens to all, shows active route)

### Active Route Selection

Signal 1 (station entry) determines the active route by:
- Which platform signal most recently changed to RED (train detected there)
- Or: manual override by pressing a button on a specific platform signal

### Resetting to Normal Mode

To remove extra peers and go back to normal mode:
- Re-pair with only one signal — overwrites all stored peers
