# Smart Road Lighting System

[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino%20Uno-00979D?logo=arduino&logoColor=white)](#)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)](#)
[![Sensors: IR + PIR](https://img.shields.io/badge/Sensors-IR%20%2B%20PIR-4CAF50)](#)

An **energy-efficient, adaptive street-lighting system** built on an Arduino Uno. The light stays in a low-power standby state and only ramps to full brightness when it detects a **vehicle** (IR sensor) or **pedestrian** (PIR sensor), then gradually dims back down.

The firmware includes a **fail-safe mode**: if both sensors appear stuck or malfunctioning, the light is forced to full brightness for road safety.

---

## Table of Contents

- [Highlights](#highlights)
- [How It Works](#how-it-works)
- [Hardware](#hardware)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Tuning & Configuration](#tuning--configuration)
- [Safety & Fail-Safe Design](#safety--fail-safe-design)
- [Limitations](#limitations)
- [Documentation](#documentation)
- [License](#license)

---

## Highlights

- ⚡ **Adaptive brightness** — dim standby by default, full power only on detection.
- 🚗 **Dual sensing** — IR for vehicles, PIR for pedestrians.
- 🌗 **Smooth dimming** — gradual PWM ramps that mimic real streetlight behaviour.
- 🛡️ **Fail-safe watchdog** — forces full brightness if sensors appear faulty.
- 🔋 **Low power** — reduces energy consumption versus always-on lighting.
- 🧩 **Highly tunable** — all timing and brightness values are configurable constants.

---

## How It Works

1. Both sensors are **polled continuously** each loop iteration.
2. On any detection, the LED **ramps smoothly to full brightness** (via PWM) and stays lit for a configurable hold time (`HOLD_TIME_MS`, default **8 s**) after the *last* detection.
3. When nothing is detected and the hold time expires, the LED **dims back to standby** rather than switching off completely.
4. A **watchdog** monitors sensor state changes. If **both sensors appear frozen** for an extended window, the firmware assumes a fault and **forces full brightness** while illuminating the onboard fault LED (pin 13).

```
        IDLE                    DETECTED                 IDLE
   ┌─────────────┐        ┌──────────────────┐     ┌─────────────┐
   │  standby    │  event │   ramp to full   │dim →│  standby    │
   │  (40 PWM)   ├────────► (255 PWM, hold)  ├────►│  (40 PWM)   │
   └─────────────┘        └──────────────────┘     └─────────────┘
```

---

## Hardware

| Component                        | Purpose                                  | Connection |
|----------------------------------|------------------------------------------|------------|
| Arduino Uno                      | Microcontroller / control logic          | —          |
| IR obstacle sensor module        | Vehicle detection (digital output)       | D2         |
| PIR motion sensor module         | Pedestrian detection (digital output)    | D3         |
| High-brightness LED / LED strip  | Illumination source                      | D9 (PWM)   |
| NPN transistor / MOSFET (driver) | Switching the real lighting load         | D9 (PWM)   |
| 220 Ω resistor                   | Current limiting for indicator LEDs      | —          |
| Onboard LED                      | Fail-safe fault indicator                | D13        |

> ⚠️ **Important:** Never drive a real streetlight directly from an Arduino pin. Always use a MOSFET/relay driver stage sized for the load current.

See **[docs/wiring.md](docs/wiring.md)** for the full pin table and circuit notes.

---

## Project Structure

```
smart-road-lighting/
├── README.md              ← You are here
├── docs/
│   ├── getting-started.md ← Install, wire, and flash guide
│   ├── wiring.md          ← Full pin table & circuit notes
│   └── firmware.md        ← Firmware architecture & operation
└── src/
    └── smart_road_lighting.ino ← Main firmware (Arduino sketch)
```

---

## Getting Started

1. Install the **Arduino IDE** ([arduino.cc](https://www.arduino.cc/en/software)).
2. Follow **[docs/getting-started.md](docs/getting-started.md)** to wire the circuit and upload the firmware.
3. Open `src/smart_road_lighting.ino` and upload it to the board.
4. Open the **Serial Monitor** at **9600 baud** to view live detection logs.

---

## Tuning & Configuration

All behaviour is controlled by constants at the top of `src/smart_road_lighting.ino`:

| Constant                | Purpose                                      | Default  |
|-------------------------|----------------------------------------------|----------|
| `BRIGHTNESS_FULL`       | PWM value when an object is detected          | `255`    |
| `BRIGHTNESS_STANDBY`    | PWM value when idle                           | `40`     |
| `HOLD_TIME_MS`          | How long the light stays full after detection | `8000 ms`|
| `SENSOR_TIMEOUT_MS`     | Watchdog base window (×20 internally)         | `3000 ms`|

See **[docs/firmware.md](docs/firmware.md)** for a deep-dive on the firmware and how to tune it.

---

## Safety & Fail-Safe Design

Street lighting is a **safety-critical application** — a dark road can be dangerous. For this reason the firmware prioritises **availability over energy savings**:

- The **default state** is a visible standby glow, not blackout.
- If **neither sensor changes state** for `SENSOR_TIMEOUT_MS × 20` (default **60 s**), the firmware assumes a sensor fault and **locks the light to full brightness**.
- The **onboard LED (D13)** lights up to indicate a fault is active.
- The system **re-evaluates** only after sensor activity resumes.

This guarantees the road stays lit even if the sensors fail.

---

## Limitations

- **No automatic recovery** — once fault mode is entered, the firmware waits for sensor activity; a power cycle is the simplest manual reset.
- **IR polarity varies** — the sketch assumes an active-LOW IR module; check your module's datasheet and flip the condition if yours is active-HIGH.
- **Single-zone logic** — this is a demo/single-pole implementation. A real deployment would use multiple nodes and a coordination layer (e.g., LoRa/MQTT).
- **No scheduling** — brightness is purely reactive; there is no time-of-day / astronomical-clock scheduling.

---

## Documentation

| Document                                        | Description                                  |
|-------------------------------------------------|----------------------------------------------|
| [Getting Started](docs/getting-started.md)     | Install dependencies, wire, and upload       |
| [Wiring](docs/wiring.md)                        | Component list, pin table, circuit notes     |
| [Firmware](docs/firmware.md)                    | Architecture, state flow, tuning reference   |

---

## License

This project is provided for demonstration and educational purposes. See the repository for license details (if any) — or add your own.
