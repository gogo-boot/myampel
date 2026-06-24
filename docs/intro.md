---
sidebar_position: 1
slug: /
---

# MyAmpel

**MyAmpel** is a battery-powered German train signal (Eisenbahnsignal) system for toy/model trains. Multiple signal masts communicate wirelessly via ESP-NOW to display realistic signal states — just like real Deutsche Bahn signals.

## What It Does

Each signal mast has:

- **Hauptsignal (Main Signal):** Shows the current track status (RED or GREEN) to the train driver at this position
- **Vorsignal (Pre-Signal):** Shows the state of the NEXT main signal ahead (YELLOW = next is red, GREEN = next is green)

When a toy train passes a signal, the main signal turns RED. The previous signal's pre-signal automatically turns YELLOW to warn approaching trains.

## Key Features

- **Wireless peer-to-peer communication** — no WiFi router or server needed
- **Battery-powered** — designed for long play sessions
- **Automatic train detection** — IR sensor detects passing trains
- **Manual override** — two buttons per signal for direct control
- **Easy pairing** — configure signal chains using buttons only, no computer needed
- **Loop or line topology** — works for circular tracks or straight lines

## Hardware

- **MCU:** ESP32-C3 (WiFi + BLE capable, low power)
- **LEDs:** 2 for main signal (red + green), 2 for pre-signal (color TBD)
- **Sensor:** IR break-beam for train detection
- **Buttons:** 2 per signal (green + red)
- **Power:** LiPo battery (~1000mAh)

## Quick Links

- [Architecture](./architecture) — System design and state machine
- [Communication](./communication) — ESP-NOW protocol and reliability
- [Hardware](./hardware) — LED, sensor, and GPIO layout
- [Lifecycle](./lifecycle) — Boot stages and power management
- [Pairing](./pairing) — How signals find each other
- [Signal Logic](./signal-logic) — German signal rules and behavior
- [Radio & Frequency](./radio) — 433 MHz, 868 MHz, 2.4 GHz comparison
