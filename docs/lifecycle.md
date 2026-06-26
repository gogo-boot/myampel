---
sidebar_position: 5
---

# Lifecycle Manager

## Overview

The LifecycleManager is a dedicated class that sequences the device through boot stages. Each stage must complete before the next begins. If a stage fails, the device shows an error pattern and halts or retries.

## Stages

```
┌──────────────────────────────────────────────────────────┐
│  BOOT       GPIO init, LED self-test (flash all LEDs)    │
├──────────────────────────────────────────────────────────┤
│  CONFIG     Load signal ID + peer MAC from NVS           │
├──────────────────────────────────────────────────────────┤
│  IDENTIFY   Flash green to confirm ready                 │
├──────────────────────────────────────────────────────────┤
│  CONNECT    Init ESP-NOW, register peers, request state  │
├──────────────────────────────────────────────────────────┤
│  RUNNING    Normal operation (LEDs + comms + sensor)     │
├──────────────────────────────────────────────────────────┤
│  SLEEP      Deep sleep (after 5 min inactivity)          │
└──────────────────────────────────────────────────────────┘
```

## Stage Details

### BOOT

- Initialize all GPIO pins (LEDs as output, buttons/sensor as input)
- LED self-test: flash all LEDs briefly (confirms hardware works)
- Duration: ~500 ms

### CONFIG

- Read from NVS (Non-Volatile Storage):
  - `signal_id` — this signal's number
  - `next_signal_mac` — MAC address of next signal (from pairing)
- If no config found: enter [pairing mode](./pairing) automatically on first boot

### IDENTIFY

- Flash green LED briefly to confirm the signal is ready
- Allows the user to visually confirm the device booted successfully
- Duration: ~500 ms

### CONNECT

- Initialize WiFi radio in ESP-NOW mode
- Register peer (next signal's MAC)
- Broadcast state request to learn current network state
- Wait up to 2 seconds for responses
- If no response: default to GREEN (safe assumption)

### RUNNING

- Main loop:
  - Monitor IR sensor for train detection
  - Monitor buttons for manual override
  - Listen for ESP-NOW messages from peers
  - Broadcast heartbeat every 2 seconds
  - Update LEDs based on state
- Stays in this state during active play

### SLEEP

- Triggered by: no button press, no train detection, no ESP-NOW messages for 5 minutes
- Enter deep sleep (all off, ~5 µA)
- Wake by: button press (GPIO wake)
- On wake: full reboot → BOOT stage again

## Failure Handling

| Stage | Failure | Behavior |
|-------|---------|----------|
| BOOT | LED not responding | Continue (can't detect easily) |
| CONFIG | NVS empty | Enter pairing mode |
| CONFIG | NVS corrupt | Clear NVS, enter pairing mode |
| CONNECT | ESP-NOW init fails | Retry 3×, then blink error pattern |
| CONNECT | No peer response | Continue with GREEN default |
| RUNNING | Peer unreachable | Keep last known state |

## Power States During Lifecycle

| Stage | Radio | LEDs | Power |
|-------|-------|------|-------|
| BOOT | Off | Self-test | ~30 mA |
| CONFIG | Off | Off | ~20 mA |
| IDENTIFY | Off | Blinking | ~25 mA |
| CONNECT | On | Off | ~80 mA (brief) |
| RUNNING | On (idle) | Signal state | ~35-50 mA |
| SLEEP | Off | Off | ~5 µA |
