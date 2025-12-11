#include <Arduino.h>

#include "config.h"
#include "util.h"

#include "rpm/rpm.h"
#include "rpm/rpm.cpp"

#include "thrust/thrust.h"
#include "thrust/thrust.cpp"

#include "motor/motor.h"
#include "motor/motor.cpp"

#include "airspeed/airspeed.h"
#include "airspeed/airspeed.cpp"

#include "power/power.h"
#include "power/power.cpp"


// look at util.h file
void setup() {
  Serial.begin(115200);
  rpm_init();
  thrust_init();
  motor_init();
  //airspeed_init();
  power_init();
  
  Serial.println("Send 'm <val>' to control motor. 0-255 or 1000-2000.");
}

void loop() {
  rpm_update();
  thrust_read();
  //airspeed_update();
  //power_update();
  
  static unsigned long rpmPrint = 0;
  if(millis() - rpmPrint > 500) {
      rpmPrint = millis();
      Serial.print("RPM: ");
      Serial.print(get_rpm());
      Serial.print(" | Total Revs: ");
      Serial.println(get_total_revs());
  }
  
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    if (input.length() > 0) {
      char cmd = input.charAt(0);
      
      if (cmd == 'm' || cmd == 'M') {
        // Parse the number after 'm'
        // Substring from index 1 to end
        String valStr = input.substring(1);
        valStr.trim(); 
        
        if (valStr.length() > 0) {
          int val = valStr.toInt();
          Serial.print("Command received: m ");
          Serial.println(val);
          motor_define_speed(val);
        } else {
          Serial.println("Error: No value provided for motor command.");
        }
      } else {
        Serial.print("Unknown command: ");
        Serial.println(input);
      }
    }
  }


  /*
  build_nmea_sentence();
  8*/
  // delay(1000);
  
  // Serial.println(build_nmea("pws", 12.34, 5.67, 70.89));
  
  // Simple status output to match user request
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    power_output(); 
  }
}
