#pragma once

// pin assignments

#define ESC_PIN  13
// pin assignments
// MOTOR_PWM_PIN removed; using ESC_PIN (13) strictly
const int THRUST_ADC_PIN  = A0;


#define HX711_DOUT_PIN  4   // HX711 DOUT (DT)
#define HX711_SCK_PIN   5   // HX711 SCK (CLK)

#define calibrationFactor  221.88f   // <--- CHANGE THIS

// ---------------- POWER CONFIG ----------------
// ESP32 ADC channels for voltage + current
#define VOLTAGE_ADC_CHANNEL   ADC1_CHANNEL_6   // GPIO34
#define CURRENT_ADC_CHANNEL   ADC1_CHANNEL_7   // GPIO35

// ADC reference values
#define ADC_MAX_COUNTS        4095.0
#define ADC_REF_VOLTAGE       3.3

// Voltage divider scaling (set your ratio here)
#define VOLTAGE_DIVIDER_SCALE 5.0   // Example: 1:5 divider → multiply by 5

// ACS758 / ACS currents sensor parameters
#define ACS_SENSITIVITY       0.04  // 40 mV per amp

// ---------------- RPM CONFIG ----------------
#define RPM_PIN 14
#define POLE_PAIRS 7 // 14 magnets / 2 = 7 pairs
