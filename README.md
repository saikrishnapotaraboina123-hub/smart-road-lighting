# Smart Road Lighting System using IR & PIR Sensors

Automated street lighting that stays dim (standby) by default and ramps to full brightness only when a vehicle (IR) or pedestrian (PIR) is detected — cutting power consumption versus always-on lighting, with a fail-safe mode that forces full brightness if the sensors appear to malfunction.

## Hardware
- Arduino Uno
- IR obstacle/vehicle sensor
- PIR motion sensor
- PWM-driven LED (via MOSFET/driver for real loads)

See [`docs/wiring.md`](./docs/wiring.md) for the full pin table and circuit notes.

## How it works
1. Both sensors are polled continuously.
2. On any detection, the LED ramps smoothly to full brightness (PWM) and stays lit for a configurable hold time (`HOLD_TIME_MS`, default 8s) after the last detection.
3. With no detection, the LED dims back down to a low standby brightness rather than switching off completely.
4. A watchdog checks whether either sensor's state has changed recently. If both sensors appear frozen for an extended period, the firmware assumes a fault and forces the LED to full brightness for safety, while lighting the onboard fault LED (pin 13).

## Flashing
1. Open `src/smart_road_lighting.ino` in Arduino IDE.
2. Select **Board:** Arduino Uno, correct **Port**.
3. Wire the circuit per `docs/wiring.md`.
4. Upload, then open Serial Monitor at 9600 baud to see detection logs.

## Tuning
| Constant | Purpose | Default |
|---|---|---|
| `BRIGHTNESS_FULL` | PWM value on detection | 255 |
| `BRIGHTNESS_STANDBY` | PWM value when idle | 40 |
| `HOLD_TIME_MS` | How long light stays full after last detection | 8000 ms |
| `SENSOR_TIMEOUT_MS` (×20 internally) | Fault-detection watchdog window | 3000 ms base |
