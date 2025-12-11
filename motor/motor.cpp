#include <Arduino.h>
#include <ESP32Servo.h>
#include "../config.h"

Servo esc;

// Constants for arming
const int ARM_PULSE = 1000;
const int ARM_TIME  = 3000; 

void motor_init() {
  Serial.println("Motor Init: Attaching ESC to PIN " + String(ESC_PIN));
  
  // 1. Attach
  esc.attach(ESC_PIN, 1000, 2000);
  
  // 2. Arming Sequence
  Serial.println("Arming ESC (sending 1000us)...");
  esc.writeMicroseconds(ARM_PULSE);
  
  // Blocking delay for safety/arming requirement
  delay(ARM_TIME);
  
  Serial.println("ESC Armed and Ready.");
}

void motor_define_speed(int speed) {
  int pulse = 1000;

  if (speed >= 0 && speed <= 255) {
    // Map 0-255 to 1000-2000
    pulse = map(speed, 0, 255, 1000, 2000);
    Serial.print("Mapping 0-255 input ");
    Serial.print(speed);
    Serial.print(" -> ");
    Serial.println(pulse);
  } else if (speed >= 1000 && speed <= 2000) {
    // Direct microseconds
    pulse = speed;
    Serial.print("Direct us input: ");
    Serial.println(pulse);
  } else {
    Serial.println("Motor command out of range (use 0-255 or 1000-2000)");
    return; // Do nothing
  }

  // Write directly
  esc.writeMicroseconds(pulse);
}
