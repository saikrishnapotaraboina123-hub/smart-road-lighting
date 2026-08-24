/*
  Smart Road Lighting System using IR & PIR Sensors
  Platform : Arduino Uno
  Sensors  : IR Sensor (vehicle detection), PIR Sensor (pedestrian detection)
  Actuator : LED strip / streetlight driven via PWM (through a MOSFET/driver for real loads)

  Behaviour:
    - IR or PIR trigger  -> LED ramps up to full brightness (PWM)
    - No detection        -> LED dims down to a low standby brightness (saves power)
    - Sensor malfunction   -> fail-safe: LED forced to full brightness for road safety

  Wiring (see docs/wiring.md for details):
    IR sensor  OUT -> D2
    PIR sensor OUT -> D3
    LED (via NPN/MOSFET driver) -> D9 (PWM)
    Status LED (onboard fault indicator) -> D13
*/

const uint8_t IR_PIN         = 2;   // IR obstacle/vehicle sensor digital output
const uint8_t PIR_PIN        = 3;   // PIR motion sensor digital output
const uint8_t LED_PWM_PIN    = 9;   // Streetlight LED, PWM capable pin
const uint8_t FAULT_LED_PIN  = 13;  // Onboard LED used as fault indicator

const uint8_t BRIGHTNESS_FULL    = 255;  // Full brightness when someone/something is detected
const uint8_t BRIGHTNESS_STANDBY = 40;   // Dim standby brightness to save power
const unsigned long HOLD_TIME_MS = 8000; // Keep light on this long after last detection

const unsigned long SENSOR_TIMEOUT_MS = 3000; // If no sensor state change for this long, flag possible malfunction

unsigned long lastDetectionTime = 0;
unsigned long lastSensorChangeTime = 0;
int lastIrState  = -1;
int lastPirState = -1;
bool faultMode = false;

int currentBrightness = BRIGHTNESS_STANDBY;

void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PWM_PIN, OUTPUT);
  pinMode(FAULT_LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println(F("Smart Road Lighting System - booting..."));

  analogWrite(LED_PWM_PIN, BRIGHTNESS_STANDBY);
  lastSensorChangeTime = millis();
}

void loop() {
  int irState  = digitalRead(IR_PIN);   // Typically LOW = object detected (module dependent)
  int pirState = digitalRead(PIR_PIN);  // HIGH = motion detected

  bool vehicleDetected    = (irState == LOW);
  bool pedestrianDetected = (pirState == HIGH);

  // Track sensor activity for fail-safe watchdog
  if (irState != lastIrState || pirState != lastPirState) {
    lastSensorChangeTime = millis();
    lastIrState = irState;
    lastPirState = pirState;
  }

  // --- Fail-safe check ---
  // If sensors appear stuck (no state change for a long time) while system
  // has been running, assume malfunction and force full brightness for safety.
  if (millis() - lastSensorChangeTime > (SENSOR_TIMEOUT_MS * 20)) {
    faultMode = true;
  }

  if (faultMode) {
    setBrightness(BRIGHTNESS_FULL);
    digitalWrite(FAULT_LED_PIN, HIGH);
    Serial.println(F("[FAULT] Sensor malfunction suspected -> forcing full brightness"));
    delay(500);
    return;
  } else {
    digitalWrite(FAULT_LED_PIN, LOW);
  }

  // --- Normal detection logic ---
  if (vehicleDetected || pedestrianDetected) {
    lastDetectionTime = millis();
    setBrightness(BRIGHTNESS_FULL);
    Serial.print(F("Detection -> Vehicle: "));
    Serial.print(vehicleDetected);
    Serial.print(F(" | Pedestrian: "));
    Serial.println(pedestrianDetected);
  } else if (millis() - lastDetectionTime > HOLD_TIME_MS) {
    setBrightness(BRIGHTNESS_STANDBY);
  }

  delay(100); // simple debounce / polling interval
}

// Smooth PWM ramp instead of an abrupt jump, mirrors real streetlight dimming behaviour.
void setBrightness(uint8_t target) {
  if (currentBrightness == target) return;

  int step = (target > currentBrightness) ? 5 : -5;
  while (currentBrightness != target) {
    currentBrightness += step;
    if ((step > 0 && currentBrightness > target) ||
        (step < 0 && currentBrightness < target)) {
      currentBrightness = target;
    }
    analogWrite(LED_PWM_PIN, currentBrightness);
    delay(10);
  }
}
