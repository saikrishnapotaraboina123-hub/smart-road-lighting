# Wiring — Smart Road Lighting System

## Components
- Arduino Uno
- IR obstacle sensor module (digital output)
- PIR motion sensor module (HC-SR501 or similar)
- High-brightness LED / LED strip (or a relay/MOSFET driver for a real streetlight)
- NPN transistor or logic-level MOSFET (e.g. IRLZ44N) if driving more than a single low-power LED
- 220Ω resistor (if using a simple indicator LED directly)
- Breadboard + jumper wires

## Connections

| Component        | Pin on module | Connects to Arduino |
|-------------------|---------------|----------------------|
| IR sensor          | VCC           | 5V                   |
| IR sensor          | GND           | GND                  |
| IR sensor          | OUT           | D2                   |
| PIR sensor         | VCC           | 5V                   |
| PIR sensor         | GND           | GND                  |
| PIR sensor         | OUT           | D3                   |
| LED driver (MOSFET gate) | Signal | D9 (PWM)            |
| LED driver         | GND           | GND (common with Arduino GND) |
| Fault indicator LED| Anode (+)     | D13 (built-in LED, no extra wiring needed) |

## Notes
- The IR module's logic polarity (active-LOW vs active-HIGH) varies by vendor — check your module's datasheet; the sketch assumes active-LOW (`OUT` goes LOW when an object/vehicle is detected). Flip the condition in `smart_road_lighting.ino` if yours is active-HIGH.
- PIR sensors need ~30–60 seconds to stabilize after power-up before readings settle.
- For driving an actual streetlight LED array, do **not** connect it directly to the Arduino pin — always use a MOSFET/relay driver stage sized for the load current.
- D9 is used for PWM because it's one of the Uno's hardware PWM-capable pins.
