#include <Arduino.h>
#include <HX711_ADC.h>
#include <EEPROM.h>

// Changed from HX711.h to HX711_ADC.h 

// ---------------------- Pin Definitions ----------------------
const int HX711_DOUT_PIN = 4;   // HX711 DOUT (DT)
const int HX711_SCK_PIN  = 5;   // HX711 SCK (CLK)

// ---------------------- HX711_ADC Object ---------------------
HX711_ADC loadCell(HX711_DOUT_PIN, HX711_SCK_PIN);

// ---------------------- EEPROM Calibration -----------------i--
struct CalData {
  float    factor;
  uint32_t magic;
};

const uint32_t CAL_MAGIC      = 0xA5A5DEAD;  // "valid" marker
const int      CAL_EEPROM_ADDR = 0;          // start address in EEPROM

// Default calibration factor used if EEPROM is empty/invalid.
// Replace this with a reasonable starting value after your first calibration.
float calibrationFactor = 100.0f;   // placeholder

// ---------------------- Timing -------------------------------
unsigned long lastPrintTime   = 0;
const unsigned long printInterval = 100;  // ms between prints

// ---------------------- Helpers: EEPROM ----------------------
void loadCalibrationFromEEPROM() {
  CalData data;
  EEPROM.get(CAL_EEPROM_ADDR, data);

  if (data.magic == CAL_MAGIC && !isnan(data.factor) && data.factor != 0.0f) {
    calibrationFactor = data.factor;
    Serial.print("Loaded calibration factor from EEPROM: ");
    Serial.println(calibrationFactor, 6);
  } else {
    Serial.println("No valid calibration in EEPROM, using default factor.");
  }
}

void saveCalibrationToEEPROM(float factor) {
  CalData data;
  data.factor = factor;
  data.magic  = CAL_MAGIC;

  EEPROM.put(CAL_EEPROM_ADDR, data);

  Serial.print("Saved calibration factor to EEPROM: ");
  Serial.println(factor, 6);
}

// ---------------------- Helper: Serial Command ---------------
// Type e.g. "CAL 123.45" in Serial Monitor to set + save new factor.
void checkSerialForCalibrationUpdate() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    line.toUpperCase();

    if (line.startsWith("CAL")) {
      // Remove "CAL" and any spaces
      line.remove(0, 3);
      line.trim();

      if (line.length() > 0) {
        float newFactor = line.toFloat();
        if (newFactor != 0.0f && !isnan(newFactor)) {
          calibrationFactor = newFactor;
          loadCell.setCalFactor(calibrationFactor);
          saveCalibrationToEEPROM(calibrationFactor);

          Serial.print("New calibration factor applied: ");
          Serial.println(calibrationFactor, 6);
        } else {
          Serial.println("Invalid calibration value.");
        }
      } else {
        Serial.println("Usage: CAL 123.45");
      }
    }
  }
}

// ---------------------- Setup ----------------------
void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println();
  Serial.println("=== Thrust Stand - HX711_ADC + Calibration Storage ===");

  // NOTE: On ESP32 you would need: EEPROM.begin(64);

  // Load calibration factor from EEPROM (if valid)
  loadCalibrationFromEEPROM();

  // Initialize load cell
  loadCell.begin();

  // Start and tare (zero) the scale
  const long stabilizingTime = 2000;  // ms to let it settle
  const bool doTare          = true;
  loadCell.start(stabilizingTime, doTare);

  if (loadCell.getTareTimeoutFlag()) {
    Serial.println("ERROR: Tare timed out, check wiring and power.");
  } else {
    Serial.println("Tare completed.");
  }

  // Apply calibration factor
  loadCell.setCalFactor(calibrationFactor);

  Serial.print("Using calibration factor: ");
  Serial.println(calibrationFactor, 6);
  Serial.println("Type:  CAL 123.45  to set and save a new factor.");
}

// ---------------------- Loop ----------------------
void loop() {
  // Must be called frequently; does non-blocking measurement + filtering
  loadCell.update();

  // Check if user typed a new calibration command
  checkSerialForCalibrationUpdate();

  // Periodic print
  unsigned long now = millis();
  if (now - lastPrintTime >= printInterval) {
    lastPrintTime = now;

    // Get value in "grams" (assuming you calibrate in grams)
    float mass_g = loadCell.getData();

    // Convert to kilograms
    float mass_kg = mass_g / 1000.0f;

    // Convert to thrust (Newtons): F = m * g
    const float g = 9.80665f;  // m/s^2
    float thrust_N = mass_kg * g;

    Serial.print("Mass: ");
    Serial.print(mass_g, 1);
    Serial.print(" g\t");

    Serial.print("Thrust: ");
    Serial.print(thrust_N, 3);
    Serial.println(" N");
  }
}


 
 
