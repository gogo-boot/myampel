---
sidebar_position: 10
---

# Physical Dimensions

## Target Scale

MyAmpel is designed to match **Brio wooden train** proportions:

- Brio track width: ~40–50 mm
- Brio train height: ~50 mm
- Brio signal height: ~130 mm
- **MyAmpel target height: ~130–150 mm**

## Signal Structure

The German H/V signal has two separate housings on one pole:

- **Hauptsignal** (main): rectangular box — shows RED or GREEN
- **Vorsignal** (pre-signal): diamond-shaped box (rotated 45°) — shows YELLOW or GREEN

```
              ┌─────┐
              │🔴   │
              │  🟢 │   ← Hauptsignal (rectangle)
              └──┬──┘
                 │      ← Gap ~30–40mm
                ╱╲
              ╱🟡🟡╲
              ╲🟢🟢╱    ← Vorsignal (diamond)
                ╲╱
                 │      ← Pole ~50–60mm
                 │
        ┌────────┴────────┐
        │      Base       │
        └─────────────────┘
```

## Component Dimensions

### Hauptsignal (Main Signal Head)

```
Width:  ~12 mm
Height: ~16 mm
Depth:  ~10 mm
LEDs:   2× 3mm (1 red, 1 green, stacked vertically)
```

### Vorsignal (Pre-Signal Head)

```
Shape:    Square rotated 45° (diamond)
Diagonal: ~22 mm
Depth:    ~10 mm
LEDs:     4× 3mm (2 yellow top, 2 green bottom)
          Arranged in 2×2 grid inside diamond
```

### Pole

```
Diameter: ~8–10 mm (hollow, carries 8 wires)
Total:    ~80–100 mm (from base to Hauptsignal)
```

### Base

```
Width:  ~50 mm
Depth:  ~30 mm
Height: ~20 mm
```

Base houses:

| Component            | Size (mm)    |
|----------------------|--------------|
| ESP32-C3 Super Mini  | 22 × 18 × 4 |
| LiPo battery 500mAh | 30 × 20 × 5 |
| TP4056 USB-C module  | 25 × 19 × 3 |
| 2× tactile buttons   | 6 × 6 × 5   |

### IR Sensor (Track Level)

```
Mounted on base sides, at ~5 mm from bottom
IR LED on one side, phototransistor on other side
Spacing: ~40–50 mm apart (across Brio track)
```

## LED Specification

**3mm LEDs** — proportional to the mast size at Brio scale.

| LED        | Color  | Count | Location      |
|------------|--------|-------|---------------|
| Main red   | Red    | 1     | Hauptsignal   |
| Main green | Green  | 1     | Hauptsignal   |
| Pre yellow | Yellow | 2     | Vorsignal     |
| Pre green  | Green  | 2     | Vorsignal     |
| **Total**  |        | **6** |               |

## Overall Assembly

```
        Total height: ~130–150 mm

        ┌───┐
        │ H │  ~16mm   Hauptsignal
        └─┬─┘
          │    ~35mm   Gap (pole)
         ╱╲
        ╲Vr╱   ~22mm   Vorsignal (diamond)
         ╲╱
          │    ~55mm   Pole to base
          │
    ┌─────┴─────┐
    │   Base    │  ~20mm
    └───────────┘
        50mm
```
