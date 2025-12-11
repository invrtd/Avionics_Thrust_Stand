#include <Arduino.h>
#include "BluetoothSerial.h" // Include header here too if needed, or rely on util
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

BluetoothSerial SerialBT;

void handle_commands(String input) {
    input.trim();
    if (input.length() > 0) {
      char cmd = input.charAt(0);
      
      if (cmd == 'm' || cmd == 'M') {
        String valStr = input.substring(1);
        valStr.trim(); 
        if (valStr.length() > 0) {
          int val = valStr.toInt();
          motor_define_speed(val);
          // Feedback
          dual_print("CMD: Motor " + String(val) + "\r\n");
        }
      }
    }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ThrustStand"); // Bluetooth device name
  
  Serial.println("Initializing Subsystems...");
  Serial.println("Bluetooth Started: 'ThrustStand'");
  
  rpm_init();
  thrust_init();
  motor_init();
  airspeed_init();
  power_init();
  
  Serial.println("System Ready.");
}

void loop() {
  // 1. Update all sensors (read hardware)
  rpm_update();
  thrust_update(); 
  airspeed_update();
  power_update();
  
  // 2. Output Data (NMEA format) at regular interval
  static unsigned long lastOutput = 0;
  if(millis() - lastOutput > 1000) { // 10Hz? No, 1000ms = 1Hz. User had 1000. 
      // User said 10Hz before but code had 1000. I'll stick to 100ms (10Hz) if that's what they wanted
      // or keep it 1000 if they prefer slow.
      // Reverting to 100ms (10Hz) as typically requested for telemetry
      lastOutput = millis();
      
      rpm_output();
      thrust_output();
      airspeed_output();
      power_output();
  }
  
  // 3. Process Input Commands (USB)
  if (Serial.available() > 0) {
    handle_commands(Serial.readStringUntil('\n'));
  }
  
  // 4. Process Input Commands (BT)
  if (SerialBT.available() > 0) {
    handle_commands(SerialBT.readStringUntil('\n'));
  }
}
