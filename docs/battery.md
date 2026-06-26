---
sidebar_position: 9
---

# Battery & Power

## ESP32-C3 Voltage Requirements

The ESP32-C3 chip operates at **2.3–3.6V**. Exceeding 3.6V damages the chip.

This means:
- A fully charged LiPo (4.2V) **cannot** connect directly to the chip
- You always need a voltage regulator between a LiPo battery and the ESP32-C3
- LiFePO4 (2.5–3.6V) is the only rechargeable chemistry that fits directly, but it requires a special charger

## Battery Comparison

| Battery         | Voltage   | Capacity  | Runtime | Direct? | Rechargeable       | BMS?                       | Charger        | Price |
|-----------------|-----------|-----------|---------|---------|--------------------|-----------------------------|----------------|-------|
| LiFePO4 14500   | 2.5–3.6V  | 600mAh   | ~15h    | ✅ Yes  | ✅ 2000+ cycles    | Optional (safe chemistry)   | ❌ Special      | ~€4   |
| LiFePO4 18650   | 2.5–3.6V  | 1500mAh  | ~37h    | ✅ Yes  | ✅ 2000+ cycles    | Optional (safe chemistry)   | ❌ Special      | ~€5   |
| **LiPo pouch**  | 3.0–4.2V  | 1000mAh  | ~25h    | ❌ LDO  | ✅ 300–500 cycles  | ✅ Yes (fire risk without)  | ✅ TP4056       | ~€3   |
| 18650 Li-ion    | 3.0–4.2V  | 3000mAh  | ~75h    | ❌ LDO  | ✅ 500–1000 cycles | ✅ Yes (overcharge risk)    | ✅ TP4056       | ~€4   |
| CR123A          | 2.0–3.0V  | 1500mAh  | ~37h    | ✅ Yes  | ❌ Disposable      | ❌ No                       | N/A            | ~€2   |
| 2× AAA NiMH    | 1.8–2.8V  | 1000mAh  | ~25h    | ⚠️ Low | ✅ 1000 cycles     | ❌ No (safe chemistry)      | Standard       | ~€3   |

Runtime estimates based on ~40mA average draw (ESP-NOW + LEDs active).

## Why LiPo Wins for MyAmpel

LiFePO4 avoids the regulator but requires a **special charger** (charges to 3.6V, not 4.2V). The standard TP4056 USB charger board cannot be used — it would overcharge and damage the cell.

For a kids' toy where USB-C charging should "just work," **LiPo + TP4056 + LDO** is the practical choice:

| Component | Purpose | Cost |
|-----------|---------|------|
| LiPo cell (3.7V, 1000mAh) | Energy storage | ~€3 |
| TP4056 USB-C module | Charging via USB-C | ~€0.50 |
| LDO regulator (e.g., ME6211) | Drops 4.2V → 3.3V | ~€0.10 |

**Total power system: ~€3.60**

## Decision

| Aspect | Choice | Rationale |
|--------|--------|-----------|
| Battery | LiPo 3.7V, 1000mAh | Small, rechargeable, widely available |
| Charging | TP4056 USB-C module | Standard, cheap, parents just plug in USB |
| Regulation | LDO (ME6211 3.3V) | Minimal dropout (~100mV), low cost |

## Power Budget

| Component | Current draw |
|-----------|-------------|
| ESP32-C3 active + ESP-NOW idle | ~20 mA |
| Main signal LED (1 lit) | ~5–10 mA |
| Pre-signal LEDs (2 lit) | ~10–20 mA |
| IR sensor | ~1 mA |
| **Total during play** | **~35–50 mA** |
| **Estimated play time (1000mAh)** | **~20–28 hours** |

Deep sleep (all off): ~5 µA → years of standby.

## Charging Behavior

- TP4056 handles charge/discharge protection (with DW01 + FS8205 combo)
- Charges at ~500mA → full charge in ~2 hours
- Can play while charging (charge-through)
- Red LED = charging, Green LED = full

## Schematic

```
USB-C ──▶ TP4056 ──▶ LiPo 3.7V ──▶ LDO (ME6211) ──▶ 3.3V ──▶ ESP32-C3
                         │                                         │
                         └──── BMS (built into TP4056 module) ─────┘
```
