#include "rpm.h"
#include "../config.h"
#include <Arduino.h>

volatile unsigned long pulseCount = 0; // Resets every interval for speed calc
volatile unsigned long totalPulses = 0; // Never resets, tracks total distance
unsigned long lastRpmTime = 0;
float currentRPM = 0.0;

// Interrupt Service Routine (ISR) - Must be in IRAM on ESP32
void IRAM_ATTR rpm_isr() {
  pulseCount++;
  totalPulses++;
}

void rpm_init() {
  pinMode(RPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), rpm_isr, RISING); 
  
  Serial.print("RPM Sensor initialized on Pin ");
  Serial.println(RPM_PIN);
}

void rpm_update() {
  unsigned long now = millis();
  const unsigned long interval = 100; // Calculate every 100ms
  
  if (now - lastRpmTime >= interval) {
    // Disable interrupts briefly to read calculation variables safely
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    
    // Time delta in seconds
    float dt = (now - lastRpmTime) / 1000.0;
    lastRpmTime = now;
    
    if (dt > 0) {
      currentRPM = (float)pulses / dt / (float)POLE_PAIRS * 60.0;
    }
  }
}

float get_rpm() {
  return currentRPM;
}

float get_total_revs() {
    // Atomic read of volatile variable
    noInterrupts();
    unsigned long tp = totalPulses;
    interrupts();
    
    return (float)tp / (float)POLE_PAIRS;
}