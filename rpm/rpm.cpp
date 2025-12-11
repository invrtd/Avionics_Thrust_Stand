#include "rpm.h"
#include "../config.h"
#include "../util.h"
#include <Arduino.h>

volatile unsigned long pulseCount = 0;
volatile unsigned long totalPulses = 0;
unsigned long lastRpmTime = 0;
float currentRPM = 0.0;

// Debounce variables
volatile unsigned long lastPulseUS = 0;
const unsigned long minPulseUS = 100; // Debounce window (100us)

// Interrupt Service Routine (ISR) with Debounce
void IRAM_ATTR rpm_isr() {
  unsigned long now = micros();
  if (now - lastPulseUS > minPulseUS) {
    pulseCount++;
    totalPulses++;
    lastPulseUS = now;
  }
}

void rpm_init() {
  pinMode(RPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), rpm_isr, FALLING); 
  
  Serial.print("RPM Sensor initialized on Pin ");
  Serial.println(RPM_PIN);
}

void rpm_update() {
  unsigned long now = millis();
  const unsigned long interval = 100; 
  
  if (now - lastRpmTime >= interval) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();
    
    float dt = (now - lastRpmTime) / 1000.0;
    lastRpmTime = now;
    
    if (dt > 0) {
      currentRPM = (float)pulses / dt / (float)POLE_PAIRS * 60.0;
    }
  }
}

// Returns RPM as an integer
int get_rpm() {
  return (int)currentRPM;
}

float get_total_revs() {
    noInterrupts();
    unsigned long tp = totalPulses;
    interrupts();
    return (float)tp / (float)POLE_PAIRS;
}

void rpm_output() {
    // $RPM,current,total*CS
    String msg = build_nmea("RPM", get_rpm(), get_total_revs());
    dual_print(msg);
}
