# Wiring Guide — Smart Road Lighting System

This guide covers the components, the complete pin table, and important circuit notes for assembling the Smart Road Lighting System.

---

## Components

| Component                          | Purpose                                       | Notes                                    |
|------------------------------------|-----------------------------------------------|------------------------------------------|
| Arduino Uno                        | Microcontroller / control logic               | Or any compatible board                   |
| IR obstacle sensor module          | Vehicle detection (digital output)            | e.g. FC-51 / HC-SR505-style              |
| PIR motion sensor module           | Pedestrian detection (digital output)         | e.g. HC-SR501                             |
| High-brightness LED / LED strip    | Illumination source                           | Use a matching LED driver                 |
| NPN transistor or logic-level MOSFET | Switch the real lighting load              | e.g. 2N2222 / IRLZ44N                     |
| 220 Ω resistor                     | Current limiting for a low-power indicator LED| Only if driving an LED directly          |
| Breadboard + jumper wires          | Build the circuit                             | —                                        |

> ⚠️ **Real streetlights draw significant current.** Never connect them directly to an Arduino pin — always use a properly-rated MOSFET or relay driver stage.

---

## Pin Connections

| Component                    | Module pin | Connects to Arduino | Notes                              |
|------------------------------|------------|----------------------|------------------------------------|
| IR sensor                    | VCC        | 5V                   | Power                               |
| IR sensor                    | GND        | GND                  | Common ground                       |
| IR sensor                    | OUT        | D2                   | Digital signal                      |
| PIR sensor                   | VCC        | 5V                   | Power                               |
| PIR sensor                   | GND        | GND                  | Common ground                       |
| PIR sensor                   | OUT        | D3                   | Digital signal                      |
| LED driver                   | Gate/Base  | D9 (PWM)            | Control signal (via 100Ω if BJT)   |
| LED driver                   | Source/Emt | GND                  | Common with Arduino GND             |
| Fault indicator LED (opt.)   | Anode (+)  | D13                  | Built-in LED — no extra wiring      |
| Fault indicator LED (opt.)   | Cathode (−)| GND                  | Only if using an external LED       |

---

## Wiring Diagram (Schematic Summary)

```
                       +5V        GND
                        │          │
      ┌─────────┐       ├──────────┤
      │  IR     │ VCC---+          │
      │ sensor  │ GND--------------┼──────┐
      │         │ OUT--------------│  D2  │ Arduino
      └─────────┘                   │      │ Uno
      ┌─────────┐                   │      │
      │  PIR    │ VCC---------------+── 5V │
      │ sensor  │ GND--------------------GND
      │         │ OUT------------------------ D3
      └─────────┘                           │
                                             │ D9 (PWM)
               ┌──────────────────────┐      │
               │  MOSFET (IRLZ44N)    │ Gate─┘
               │  Drain ──► load ──► +V       (LED strip / streetlight)
               │  Source ──────────── GND
               └──────────────────────┘
```

---

## Circuit Notes

### IR sensor polarity
IR modules vary by vendor — some output **LOW when a vehicle is detected** (active-LOW), others output **HIGH** (active-HIGH). The sketch assumes **active-LOW** (`OUT` goes LOW on detection). If your module is active-HIGH, flip the condition in `smart_road_lighting.ino`:

```cpp
bool vehicleDetected = (irState == HIGH);   // for active-HIGH modules
```

### PIR warm-up time
PIR sensors need roughly **30–60 seconds** after power-up to stabilise before readings settle. Expect occasional false triggers during this window — this is normal.

### Driving a real lighting load
- The Arduino pin can only source/sink a few tens of milliamps — **insufficient** for a streetlight.
- Use a **logic-level MOSFET** (e.g. IRLZ44N) or a relay module rated for your load's voltage and current.
- Always connect the driver's **Ground to the Arduino GND** for a common reference.

### PWM pin selection
**D9** is used because it is one of the Arduino Uno's **hardware PWM-capable pins**. Using a hardware PWM pin produces clean, glitch-free dimming.

### External fault LED
The sketch uses the **onboard LED (D13)** by default, so no external wiring is needed. If you prefer a brighter external LED, connect it to D13 with a **220 Ω resistor** in series.

---

## Checklist Before Powering On

- [ ] All VCC pins connected to 5V (not 3.3V)
- [ ] All GND pins connected to a **common ground**
- [ ] IR `OUT` → D2, PIR `OUT` → D3
- [ ] LED driver control → D9 (PWM)
- [ ] No component powered while wires are being changed
- [ ] Load driver rated for your LED/streetlight current

---

## Next Steps

Once wired, follow the **[Getting Started Guide](getting-started.md)** to upload the firmware and test the system.
