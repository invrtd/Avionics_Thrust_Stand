#pragma once 

// Voltage sensor: 0–24 V → 0–5 V
const float VOLTAGE_SCALE = 24.0 / 5.0;

void power_init();
void power_update();
void power_output();

// Latest readings
extern float V_batt;
extern float I_batt;
extern float P_batt;
