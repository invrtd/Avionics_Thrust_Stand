#include "thrust.h"
#include "../config.h"
#include <Arduino.h>
#include "HX711.h"


// ------------------- Pin Definitions -------------------
const int HX711_DOUT = 4;   
const int HX711_SCK  = 5;

// ------------------- HX711 Object -------------------
HX711 scale;

// ------------------- Calibration Factor -------------------
float calibration_factor = -1098.0;   // Replace with your calibrated value

// ------------------- Initialization Function -------------------
void thrust_init() {
    Serial.println("Initializing thrust measurement...");

    scale.begin(HX711_DOUT, HX711_SCK);

    // Apply calibration factor
    scale.set_scale(calibration_factor);

    // Zero the load cell
    scale.tare();

    Serial.println("Thrust measurement initialized.");
}
    
// ------------------- Read Function -------------------
float thrust_read() {
    // Average 5 samples for noise reduction
    float thrust = scale.get_units(5); 
    return thrust;   // Units depend on calibration (N or lbf)
}



 
 
