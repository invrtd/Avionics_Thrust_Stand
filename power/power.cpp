#include "power.h"
#include "../config.h"
#include <Arduino.h>
#include <driver/adc.h>

// Global readings
float V_batt = 0;
float I_batt = 0;
float P_batt = 0;

// Internal calibration value
static float ACS_ZERO = 2.5;

// ----------------------------------------------------
// Calibrate ACS current sensor zero offset
// ----------------------------------------------------
void calibrateACS() {
    long sum = 0;

    for (int i = 0; i < 500; i++) {
        sum += adc1_get_raw(CURRENT_ADC_CHANNEL);
        delay(2);
    }

    int raw_zero = sum / 500;

    ACS_ZERO = (raw_zero / ADC_MAX_COUNTS) * ADC_REF_VOLTAGE;
}

// ----------------------------------------------------
// Voltage reading FROM CONFIGURED ADC CHANNEL
// ----------------------------------------------------
float read_voltage_sensor() {
    int raw = adc1_get_raw(VOLTAGE_ADC_CHANNEL);
    float vin = (raw / ADC_MAX_COUNTS) * ADC_REF_VOLTAGE;

    return vin * VOLTAGE_DIVIDER_SCALE;  // Apply voltage divider ratio
}

// ----------------------------------------------------
// Current reading FROM CONFIGURED ADC CHANNEL
// ----------------------------------------------------
float read_current_sensor() {
    int raw = adc1_get_raw(CURRENT_ADC_CHANNEL);
    float vout = (raw / ADC_MAX_COUNTS) * ADC_REF_VOLTAGE;

    return (vout - ACS_ZERO) / ACS_SENSITIVITY;
}

// ----------------------------------------------------
// INIT FUNCTION
// ----------------------------------------------------
void power_init() {
    Serial.println("Power subsystem init...");

    // Configure ESP32 ADC width
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configure attenuation on both channels (up to ~3.2V input)
    adc1_config_channel_atten(VOLTAGE_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(CURRENT_ADC_CHANNEL, ADC_ATTEN_DB_11);

    // Calibrate current sensor offset
    calibrateACS();

    Serial.println("Power subsystem ready.");
}

// ----------------------------------------------------
// UPDATE FUNCTION — compute V, I, and Power
// ----------------------------------------------------
void power_update() {
    V_batt = read_voltage_sensor();
    I_batt = read_current_sensor();
    P_batt = V_batt * I_batt;
}

// ----------------------------------------------------
// OUTPUT FUNCTION — your existing NMEA formatting
// ----------------------------------------------------
void power_output() {
    String out = build_nmea("power", V_batt, I_batt, P_batt);
    Serial.println(out);
}
