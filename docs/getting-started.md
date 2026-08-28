# Getting Started — Smart Road Lighting System

Follow this guide to set up your development environment, wire the circuit, and flash the firmware to an Arduino Uno.

---

## 1. Install the Arduino IDE

Download and install the **Arduino IDE** from the official site:

- [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

Install the version for your operating system (Windows, macOS, or Linux).

---

## 2. Clone / Obtain the Source

If you have Git and SSH set up:

```bash
git clone git@github.com:saikrishnapotaraboina123-hub/smart-road-lighting.git
cd smart-road-lighting
```

Alternatively, download the repository as a ZIP from GitHub and extract it.

---

## 3. Connect the Hardware

Wire the components according to [wiring.md](wiring.md). A quick summary:

| Component                        | Arduino Pin |
|----------------------------------|-------------|
| IR sensor `OUT`                   | D2          |
| PIR sensor `OUT`                  | D3          |
| LED / driver signal               | D9 (PWM)    |
| Fault indicator LED (optional)    | D13         |

> ⚠️ **Safety first:** Power the board down (or unplug USB) before making or changing wire connections.

---

## 4. Open the Sketch

1. Launch the **Arduino IDE**.
2. Open **File → Open…** and select:
   ```
   src/smart_road_lighting.ino
   ```
3. The sketch and its folder will load.

---

## 5. Select Board & Port

1. Go to **Tools → Board → Arduino AVR Boards** and choose **Arduino Uno**.
2. Go to **Tools → Port** and select the port your Uno is connected to (e.g., `COM3` on Windows).
3. If the wrong board/port shows up, disconnect/reconnect the board and refresh the port list.

---

## 6. Upload the Firmware

1. Click the **Upload** button (the right arrow icon) in the toolbar, or press **Ctrl+U**.
2. Wait for `Done uploading` to appear in the status area — this usually takes a few seconds.

---

## 7. Monitor the Output (Optional)

1. Open the **Serial Monitor** via **Tools → Serial Monitor**, or press **Ctrl+Shift+M**.
2. Set the baud rate to **9600**.
3. You should see boot and detection logs, e.g.:

```
Smart Road Lighting System - booting...
Detection -> Vehicle: 0 | Pedestrian: 1
[FAULT] Sensor malfunction suspected -> forcing full brightness
```

---

## 8. Test It

- **Vehicles:** move an object (e.g., your hand) in front of the IR sensor → the LED should ramp to full brightness.
- **Pedestrians:** walk past the PIR sensor → the LED should ramp up.
- **Standby:** with nothing moving for `HOLD_TIME_MS` (default 8 s), the LED should dim to standby (PWM 40).

---

## Troubleshooting

| Problem                                   | Likely Cause / Fix                                      |
|-------------------------------------------|---------------------------------------------------------|
| Board not detected                         | Install USB drivers; try a different USB cable/port      |
| Upload fails with `avrdude` error          | Port in use by Serial Monitor; close it and re-upload    |
| IR sensor triggers constantly              | IR polarity may be active-HIGH — see [wiring.md](wiring.md) |
| PIR triggers erratically for first minute  | Normal warm-up; wait 30–60 s for the sensor to stabilise |
| Nothing happens when motion is detected    | Check PIR/IR `OUT` connections and that VCC/GND are wired |
