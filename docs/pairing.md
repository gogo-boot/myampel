---
sidebar_position: 6
---

# Pairing

## Overview

Pairing connects two signals so they know each other. Signal A needs to know: "Signal B is my next signal." This is done entirely with the two buttons on each device — no computer, phone, or app needed.

## Concept

Each signal only needs **one peer**: the next signal in the chain. Pairing stores the next signal's MAC address in NVS.

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
1. Enter pairing mode again
2. New pairing overwrites the stored MAC in NVS
3. Old pairing is forgotten

## Timeout

If no pairing is completed within 30 seconds of entering pairing mode:
- Exit pairing mode
- Return to previous state (RUNNING or CONNECT)
- LEDs return to normal

## First Boot (Unpaired)

If a signal has never been paired (NVS empty):
- After BOOT and CONFIG stages detect empty NVS
- Automatically enter pairing mode
- Stay in pairing mode until paired or powered off
