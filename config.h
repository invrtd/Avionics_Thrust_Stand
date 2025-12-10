#pragma once

// pin assignments
const int MOTOR_PWM_PIN   = 5;
const int MOTOR_RPM_PIN  = 2;
const int THRUST_ADC_PIN  = A0;
const int VOLTAGE_READ_PIN = A1;
const int CURRENT_READ_PIN = A2;
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

// etc...
