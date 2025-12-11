#include <Arduino.h>
#include <HX711_ADC.h>
#include "../config.h"
#include "../util.h"

//load cell
HX711_ADC loadCell(HX711_DOUT_PIN, HX711_SCK_PIN);

// Storage
float current_mass_g = 0.0;
float current_thrust_N = 0.0;

//setup
void thrust_init() {
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
void thrust_update() {
  loadCell.update();
  
  // Get data whenever available or on specific interval? 
  // HX711_ADC handles its own timing mostly, getData returns last read.
  // We'll read it every cycle.
  
  float mass_g = loadCell.getData(); // Get value
  float mass_kg = mass_g / 1000.0f;
  const float g = 9.80665f;
  float thrust_N = mass_kg * g;
  
  // Invert if necessary (thrust usually pushes, might register negative depending on mount)
  // Previous code inverted output (-thrust_N), so we'll store raw.
  // Actually, let's store it as user previously saw it. User printed -thrust_N.
  // So "Thrust" is -1 * calculated if load cell is compressed/pulled opposite.
  
  current_mass_g = mass_g;
  current_thrust_N = -thrust_N; 
}

float get_thrust() {
    return current_thrust_N;
}

float get_mass() {
    return current_mass_g;
}

void thrust_output() {
    // $THRUST,mass_g,thrust_N*CS
    // Match CSV format roughly but in NMEA
    String msg = build_nmea("THRUST", current_mass_g, current_thrust_N);
    dual_print(msg);
}
