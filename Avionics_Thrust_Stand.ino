#include <Arduino.h>

#include "config.h"
#include "util.h"

#include "rpm/rpm.h"
#include "rpm/rpm.cpp" // Include impl if needed by Arduino IDE quirk, or just link. 
// Standard arduino structure often needs CPP included if not using library structure properly.
// Assuming user had them included before.

#include "thrust/thrust.h"
#include "thrust/thrust.cpp"

#include "motor/motor.h"
#include "motor/motor.cpp"

#include "airspeed/airspeed.h"
#include "airspeed/airspeed.cpp"

#include "power/power.h"
#include "power/power.cpp"


void setup() {
  Serial.begin(115200);
  
  Serial.println("Initializing Subsystems...");
  
  rpm_init();
  thrust_init();
  motor_init();
  airspeed_init();
  power_init();
  
  Serial.println("System Ready.");
  Serial.println("Commands: m <0-255>");
}

void loop() {
  // 1. Update all sensors (read hardware)
  rpm_update();
  thrust_update(); // Renamed from thrust_read
  airspeed_update();
  power_update();
  
  // 2. Output Data (NMEA format) at regular interval
  static unsigned long lastOutput = 0;
  if(millis() - lastOutput > 100) { // 10Hz output
      lastOutput = millis();
      
      rpm_output();
      thrust_output();
      airspeed_output();
      power_output();
  }
  
  // 3. Process Input Commands
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      char cmd = input.charAt(0);
      
      if (cmd == 'm' || cmd == 'M') {
        String valStr = input.substring(1);
        valStr.trim(); 
        if (valStr.length() > 0) {
          int val = valStr.toInt();
          // Serial.print("CMD: m "); Serial.println(val); // Optional echo
          motor_define_speed(val);
        }
      } else {
         // Unknown command
      }
    }
  }
}
