---
sidebar_position: 11
---

# Shopping List

## Per Signal Mast

| Component | With XIAO | Without XIAO |
|-----------|-----------|--------------|
| MCU board | XIAO ESP32C3 — €5.00 | ESP32-C3 Super Mini — €2.50 |
| Battery charger | ✅ Included in XIAO | TP4056 USB-C module — €0.50 |
| LiPo battery 3.7V 500mAh | €3.00 | €3.00 |
| 3mm Red LED ×1 | €0.02 | €0.02 |
| 3mm Green LED ×3 | €0.06 | €0.06 |
| 3mm Yellow LED ×2 | €0.04 | €0.04 |
| 5mm IR LED (940nm) ×1 | €0.05 | €0.05 |
| 5mm IR Phototransistor ×1 | €0.05 | €0.05 |
| 220Ω resistors ×6 | €0.06 | €0.06 |
| 100Ω resistor ×1 (IR LED) | €0.01 | €0.01 |
| 10kΩ resistor ×1 (IR pull-up) | €0.01 | €0.01 |
| 6mm tactile buttons ×2 | €0.10 | €0.10 |
| **Total per signal** | **€8.40** | **€6.40** |

## 4-Signal Setup

| Item | With XIAO | Without XIAO |
|------|-----------|--------------|
| 4× signal masts | €33.60 | €25.60 |
| Dupont jumper wires (F-F, 20cm) | €2.00 | €2.00 |
| Heat shrink tubing (assorted) | €3.00 | €3.00 |
| USB-C cable | €2.00 | €2.00 |
| **Grand total** | **€40.60** | **€32.60** |

:::warning Insulate your resistors
Bare resistor legs can short each other inside the narrow mast pole. Cover each resistor with heat shrink tubing to prevent shorts — a short bypasses the resistor and can burn out LEDs or damage GPIO pins.
:::

## Trade-offs

| | With XIAO | Without XIAO |
|---|---|---|
| Boards in base | 1 | 2 |
| Base size | Smaller | Larger |
| Wiring complexity | Simpler | More wires |
| Charge speed | 100mA (~5h for 500mAh) | 500mA (~1h for 500mAh) |
| Price per signal | +€2 more | Cheapest |

## Optional / Future

| Component | Price | Purpose |
|-----------|-------|---------|
| Custom PCB (JLCPCB, 5 pcs) | ~€5 | Production quality |
| 3D printed mast housing | ~€2/mast | Enclosure |
| SX1262 module (433 MHz) | ~€3 | Future sub-GHz radio upgrade |
