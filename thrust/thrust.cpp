#include <Arduino.h>
#include <HX711_ADC.h>
//Pin definitions


//calibration factor (may change)


//load cell
HX711_ADC loadCell(HX711_DOUT_PIN, HX711_SCK_PIN);

//setup
void thrust_init() {
  // Serial.begin(9600); // Removed: Main setup handles Serial
  // delay(500);
  Serial.println();
  Serial.println("=== HX711_ADC Thrust Stand ===");

  loadCell.begin();

  //tare
  const long stabilizingTime = 2000;  
  const bool doTare = true;
  loadCell.start(stabilizingTime, doTare);

  if (loadCell.getTareTimeoutFlag()) {
    Serial.println("ERROR: Tare timeout — check wiring!");
  } else {
    Serial.println("Tare complete.");
  }

  loadCell.setCalFactor(calibrationFactor);

  Serial.println("Setup complete. Reading thrust...");
}
//loop
void thrust_read() {

  loadCell.update();

  static unsigned long lastPrintTime = 0;
  const unsigned long printInterval = 1000; // ms (10 Hz)

  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();

    //mass in grams
    float mass_g = loadCell.getData();

    //kilogram conversion
    float mass_kg = mass_g / 1000.0f;

    //mass → force (Newton’s law F = m·g)
    const float g = 9.80665f;
    float thrust_N = mass_kg * g;

    //printing in CSV format (mass, thrust)
    Serial.print(mass_g, 2);
    Serial.print(",");
    Serial.println(-thrust_N, 3);
  }
}


 
 
