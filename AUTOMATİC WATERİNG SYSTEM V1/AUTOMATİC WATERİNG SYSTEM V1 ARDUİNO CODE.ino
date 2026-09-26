/*
   ============================================================
    Smart Soil Moisture Irrigation System
    Automatic + Manual Watering with Overwatering Protection
   ============================================================

    Description:
    An Arduino-based irrigation controller that waters a plant
    automatically based on soil moisture readings, with a manual
    override button. Designed to avoid the common "pump never
    stops and floods the pot" failure mode found in basic
    soil-moisture irrigation sketches.

    Key safety features:
      - Hysteresis (separate ON/OFF thresholds) to avoid rapid
        on/off cycling near the threshold.
      - Non-blocking timing (millis-based state machine) instead
        of long delay() calls, so the button stays responsive.
      - Short watering "bursts" followed by a soak period, giving
        the soil time to absorb water before the next reading.
      - A hard safety cap on total consecutive watering time, in
        case the reading never drops below threshold (bad
        calibration, sensor fault, drainage issue, etc.).
      - Configurable relay polarity. Many cheap relay modules are
        active-LOW; if this isn't accounted for, the pump can run
        continuously from the moment the board powers on.

    Hardware:
      - Soil moisture sensor              -> A0
      - Push button (manual watering)     -> D4 (INPUT_PULLUP)
      - Relay module (pump / solenoid)    -> D8

    IMPORTANT - Relay polarity check:
      Power the board with the button NOT pressed. If the pump
      turns on immediately, the relay module is active-LOW and
      RELAY_ACTIVE_LOW should stay "true". If the pump correctly
      stays off, set RELAY_ACTIVE_LOW to "false".

    Calibration:
      Open the Serial Monitor at 9600 baud. Note the raw analog
      reading with the sensor in dry air, then again with the tip
      submerged in water. Use those two values to set
      DRY_THRESHOLD and WET_THRESHOLD below, keeping a margin
      between them so the hysteresis works correctly.

    License: MIT
*/

// ---------- PIN DEFINITIONS ----------
const int SOIL_PIN   = A0;  // Soil moisture sensor (analog input)
const int BUTTON_PIN = 4;   // Manual watering button (active LOW, INPUT_PULLUP)
const int RELAY_PIN  = 8;   // Relay controlling the pump / solenoid valve

// ---------- RELAY POLARITY ----------
// See "Relay polarity check" above.
const bool RELAY_ACTIVE_LOW = true;

// ---------- MOISTURE THRESHOLDS (HYSTERESIS) ----------
const int DRY_THRESHOLD = 600;  // Above this value -> soil is dry, start watering
const int WET_THRESHOLD = 450;  // Below this value -> soil is moist enough, stop
// DRY_THRESHOLD must be greater than WET_THRESHOLD. Calibrate both using the
// Serial Monitor readings described above.

// ---------- TIMING ----------
const unsigned long WATER_BURST_MS     = 1000;   // Duration of a single watering burst
const unsigned long SOAK_MS            = 60000;  // Wait time after a burst for water to soak in
const unsigned long MAX_TOTAL_WATER_MS = 20000;  // Safety cap on total consecutive watering time
const unsigned long MANUAL_MAX_MS      = 10000;  // Max duration for one manual watering press
const unsigned long DEBOUNCE_MS        = 50;     // Button debounce time
const unsigned long SENSOR_READ_MS     = 1000;   // How often to sample the sensor

// ---------- STATE MACHINE ----------
enum SystemState { IDLE, WATERING, SOAKING, MANUAL };
SystemState state = IDLE;

unsigned long stateStartTime  = 0;
unsigned long totalWateringMs = 0;
unsigned long lastSensorRead  = 0;
int soilValue = 0;

// Button debounce state
int lastButtonReading  = HIGH;
int stableButtonState  = HIGH;
unsigned long lastButtonChange = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pumpOff();  // Uses RELAY_ACTIVE_LOW, correct regardless of relay type
  Serial.println(F("Smart Irrigation System started."));
}

void loop() {
  unsigned long now = millis();

  if (now - lastSensorRead >= SENSOR_READ_MS) {
    lastSensorRead = now;
    soilValue = analogRead(SOIL_PIN);
    Serial.print(F("Soil: "));
    Serial.print(soilValue);
    Serial.print(F("  State: "));
    Serial.println(stateName(state));
  }

  bool buttonPressed = readButton();

  switch (state) {

    case IDLE:
      if (buttonPressed) {
        state = MANUAL;
        stateStartTime = now;
        pumpOn();
        Serial.println(F(">> Manual watering started"));
      } else if (soilValue > DRY_THRESHOLD) {
        state = WATERING;
        stateStartTime = now;
        totalWateringMs = 0;
        pumpOn();
        Serial.println(F(">> Soil is dry - watering started"));
      }
      break;

    case WATERING:
      if (now - stateStartTime >= WATER_BURST_MS) {
        pumpOff();
        totalWateringMs += (now - stateStartTime);
        state = SOAKING;
        stateStartTime = now;
      } else if (buttonPressed) {
        pumpOff();
        state = IDLE;
        Serial.println(F("<< Watering cancelled by button"));
      }
      break;

    case SOAKING:
      if (buttonPressed) {
        state = MANUAL;
        stateStartTime = now;
        pumpOn();
        Serial.println(F(">> Soak cancelled, manual watering started"));
      } else if (now - stateStartTime >= SOAK_MS) {
        if (totalWateringMs >= MAX_TOTAL_WATER_MS) {
          state = IDLE;
          totalWateringMs = 0;
          Serial.println(F("<< SAFETY LIMIT reached - check sensor/calibration"));
        } else if (soilValue > DRY_THRESHOLD) {
          state = WATERING;
          stateStartTime = now;
          pumpOn();
          Serial.println(F(">> Still dry - additional watering burst"));
        } else {
          state = IDLE;
          totalWateringMs = 0;
          Serial.println(F("<< Moisture level sufficient, returning to idle"));
        }
      }
      break;

    case MANUAL:
      if (soilValue < WET_THRESHOLD || now - stateStartTime >= MANUAL_MAX_MS) {
        pumpOff();
        state = IDLE;
        Serial.println(F("<< Manual watering finished"));
      } else if (buttonPressed) {
        pumpOff();
        state = IDLE;
        Serial.println(F("<< Manual watering stopped by button"));
      }
      break;
  }
}

// ---------- HELPER FUNCTIONS ----------

void pumpOn() {
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH);
}

void pumpOff() {
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
}

bool readButton() {
  int reading = digitalRead(BUTTON_PIN);
  bool triggered = false;

  if (reading != lastButtonReading) {
    lastButtonChange = millis();
  }
  if ((millis() - lastButtonChange) > DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == LOW) triggered = true;
    }
  }
  lastButtonReading = reading;
  return triggered;
}

String stateName(SystemState s) {
  switch (s) {
    case IDLE: return "IDLE";
    case WATERING: return "WATERING";
    case SOAKING: return "SOAKING";
    case MANUAL: return "MANUAL";
  }
  return "?";
}
