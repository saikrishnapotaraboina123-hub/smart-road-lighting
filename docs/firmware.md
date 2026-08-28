# Firmware — Smart Road Lighting System

This document explains the architecture, state flow, and tuning knobs of the Arduino firmware in `src/smart_road_lighting.ino`.

---

## Overview

The sketch implements a **reactive, fail-safe** street-lighting controller. It continuously polls two sensors — an **IR sensor** (vehicles) and a **PIR sensor** (pedestrians) — and drives a **PWM-dimmed LED** as the light source.

Three goals drive the design:

1. **Save energy** — stay dim when the road is empty.
2. **Respond fast** — light up immediately when something approaches.
3. **Stay safe** — force full brightness if the sensors fail.

---

## Hardware Pins

Defined as constants at the top of the sketch:

| Constant          | Pin | Role                                   |
|-------------------|-----|----------------------------------------|
| `IR_PIN`          | D2  | IR / vehicle sensor (digital input)    |
| `PIR_PIN`         | D3  | PIR / pedestrian sensor (digital input)|
| `LED_PWM_PIN`     | D9  | Streetlight PWM output (via driver)    |
| `FAULT_LED_PIN`   | D13 | Onboard fault indicator LED            |

---

## Core Logic Flow

```
                    ┌────────────────────────────┐
                    │          setup()           │
                    │ pins, serial, standby PWM  │
                    └─────────────┬──────────────┘
                                  ▼
                    ┌────────────────────────────┐
                    │          loop()            │
                    │  read IR + PIR             │
                    └─────────────┬──────────────┘
                                  ▼
              ┌───────────────────┴───────────────────┐
              │       Sensors changed state?          │
              └───────────────┬───────────────────────┘
                              ▼
              ┌────────────────────────────┐
              │      watchdog check        │
              │ idle time > timeout×20 ?   │
              └─────────────┬──────────────┘
                     ┌──────┴──────┐
                     ▼             ▼
              ┌──────────┐   ┌──────────────┐
              │ FAULT    │   │ Normal logic │
              │ mode     │   │ detection    │
              └──────────┘   └──────┬───────┘
```

### 1. Setup phase (`setup()`)

- Configures all pins as input/output.
- Starts the Serial port at **9600 baud**.
- Sets the LED to standby brightness immediately (avoids a bright flash on boot).
- Initialises the watchdog timestamp.

### 2. Sensor polling (`loop()`)

- Reads the IR and PIR pins every iteration.
- Maps the raw states to meaningful booleans:
  - `vehicleDetected = (irState == LOW)` — the sketch assumes an **active-LOW** IR module.
  - `pedestrianDetected = (pirState == HIGH)`.
- Records `millis()` whenever either sensor **changes state** (this feeds the watchdog).

### 3. Fail-safe watchdog

- If **no sensor state change** occurs for `SENSOR_TIMEOUT_MS × 20` (default 3 000 × 20 = **60 s**), the firmware enters `faultMode`:
  - Forces the LED to **full brightness**.
  - Lights the onboard fault LED (D13).
  - Logs to Serial.
- Fault mode **latches** (stays active) until sensor activity resumes.

### 4. Normal detection logic

- On any detection, records `lastDetectionTime` and ramps to **full brightness**.
- Once `HOLD_TIME_MS` (default **8 s**) passes since the *last* detection, the LED **dims to standby**.

### 5. Smooth dimming (`setBrightness()`)

- Instead of jumping instantly, brightness moves in **steps of ±5** every 10 ms, producing a smooth ramp that mimics real streetlight behaviour.
- If the target is already reached, the function returns immediately (no-op).

---

## Configuration Constants

| Constant               | Default | Description                                   |
|------------------------|---------|-----------------------------------------------|
| `BRIGHTNESS_FULL`      | `255`   | PWM value used when detecting. Max = 255.     |
| `BRIGHTNESS_STANDBY`   | `40`    | PWM value when idle (0–255).                  |
| `HOLD_TIME_MS`         | `8000`  | Time the light stays full after last detection.|
| `SENSOR_TIMEOUT_MS`    | `3000`  | Watchdog base window; multiplied by 20 internally.|

### Suggested tuning

- **Reducing standby brightness** lowers idle power but may look "off". Keep it > 0 for safety visibility.
- **Increasing `HOLD_TIME_MS`** keeps the road lit longer; good for frequently-used crossings. Decreasing it saves more energy.
- **Watchdog sensitivity** is `SENSOR_TIMEOUT_MS × 20`. Lowering `SENSOR_TIMEOUT_MS` makes the fail-safe trigger sooner — conservative and safer, but it may false-trigger on a genuinely quiet road.

---

## Key Design Decisions

- **Standby > blackout:** a low standby glow keeps a minimum light level for safety even with zero detection.
- **Latching fault mode:** errors are rarely self-healing; keeping the light on until real activity is observed is safer than flickering on/off.
- **Dual-sensor redundancy:** using both IR and PIR means a failure of *one* sensor still gives partial coverage.
- **PWM hardware pin (D9):** chosen because it's one of the Uno's native PWM-capable pins.

---

## Extending the Firmware

Possible improvements (not yet implemented):

- **Auto-recovery timer** — exit fault mode after a fixed period instead of waiting indefinitely.
- **Astronomical / time-of-day scheduling** — only operate at night.
- **Multi-zone coordination** — communicate between poles via serial, LoRa, or MQTT.
- **Duty-cycle / energy metering** — track on-time to report energy savings.
- **Configurable IR polarity** — a runtime option instead of a compile-time assumption.
