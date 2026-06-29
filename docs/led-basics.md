---
sidebar_position: 13
---

# LED Basics

## How an LED Circuit Works

An LED needs two things: the right voltage and limited current. The resistor provides both.

```
GPIO (3.3V) ──[220Ω]── LED ── GND
```

### Voltage sharing

The 3.3V splits between the resistor and the LED:

| Component | Voltage | Why |
|-----------|---------|-----|
| LED | 2.0V | Fixed — set by the LED's material (color) |
| Resistor | 1.3V | Leftover (3.3V − 2.0V) |
| **Total** | **3.3V** | Must add up to supply voltage |

### Current limiting

The resistor limits current using only **its own** voltage (the leftover):

```
I = V_resistor / R = 1.3V / 220Ω = ~6 mA
```

Not 3.3V / 220Ω — because the resistor only sees 1.3V, not the full supply.

Without the resistor → nothing limits current → LED burns out.

## Forward Voltage by Color

| Color | Wavelength | Forward Voltage | Resistor (@ 3.3V) | Current | Works? |
|-------|-----------|----------------|-------------------|---------|--------|
| Red | 620–750nm | ~1.8–2.0V | 220Ω | ~6 mA | ✅ Yes |
| Yellow | 580–620nm | ~2.0–2.2V | 220Ω | ~5 mA | ✅ Yes |
| Green | 495–570nm | ~2.0–2.2V | 220Ω | ~5 mA | ✅ Yes |
| IR | 940nm (invisible) | ~1.2–1.5V | 100Ω | ~18 mA | ✅ Yes |
| Blue | 450–495nm | ~3.0–3.4V | 220Ω | ~0.5 mA | ⚠️ Very dim |
| White | mixed | ~3.0–3.5V | 220Ω | ~0 mA | ❌ Won't light |

Wavelength (nm) is the "color" of light. Lower numbers = bluer, higher numbers = redder. IR at 940nm is beyond what human eyes can see — invisible, but detectable by a phototransistor.

## LED Diameter: 3mm vs 5mm

| Property | 3mm | 5mm |
|----------|-----|-----|
| Brightness | Lower | Higher |
| Beam angle | Wider (~30–60°) | Narrower (~15–30°) |
| Forward voltage | Same | Same |
| Current draw | Same (with same resistor) | Same |
| Best for | Signal LEDs (proportional to mast) | IR sensor (stronger beam) |

### Chosen sizes for MyAmpel

| Component | Diameter | Why |
|-----------|----------|-----|
| Red, Yellow, Green LEDs | **3mm** | Proportional to Brio-scale signal mast |
| IR LED (transmitter) | **5mm** | Stronger beam for 5cm reflective detection |
| IR Phototransistor | **5mm** | Larger surface = more sensitive to reflections |

### IR LED: 3mm vs 5mm comparison

If space is tight and you prefer 3mm for everything:

| IR LED size | Resistor | Current | Effective range |
|-------------|----------|---------|-----------------|
| 5mm | 100Ω | ~18 mA | ~5–10 cm |
| 3mm | 68Ω | ~27 mA | ~3–7 cm |

Both work for train detection — 5mm gives more margin at 5cm distance.

## IR LED and Receiver

### IR LED (Transmitter)

Same principle as visible LEDs, but lower forward voltage (~1.2–1.5V). Uses a 100Ω resistor for higher current (~18mA) to get stronger beam for reflective detection.

```
GPIO9 (PWM 38kHz) ──[100Ω]── IR LED (5mm, 940nm) ── GND
```

### IR Phototransistor (Receiver)

Not an LED — it's a light-sensitive transistor. When IR light hits it, it conducts current (pulls the pin LOW).

```
3.3V ──[10kΩ pull-up]──┬── GPIO8 (input)
                        │
              IR Phototransistor (5mm)
                        │
                       GND
```

- No IR light → GPIO8 reads HIGH (pulled up by 10kΩ)
- IR reflection from train → GPIO8 reads LOW (phototransistor conducts)

## MyAmpel Summary

| Component | Size | Resistor | Current | Pin |
|-----------|------|----------|---------|-----|
| Red LED (main) | 3mm | 220Ω | ~6 mA | GPIO0 |
| Green LED (main) | 3mm | 220Ω | ~5 mA | GPIO1 |
| Yellow LED ×2 (pre) | 3mm | 220Ω each | ~5 mA | GPIO3, GPIO10 |
| Green LED ×2 (pre) | 3mm | 220Ω each | ~5 mA | GPIO4, GPIO5 |
| IR LED (transmitter) | 5mm | 100Ω | ~18 mA | GPIO9 |
| IR Phototransistor | 5mm | 10kΩ pull-up | — | GPIO8 |
