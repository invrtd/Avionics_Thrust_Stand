#pragma once 

const int RC_MIN_PULSE = 1000;        // Minimum pulse width from receiver (in microseconds)
const int RC_MAX_PULSE = 2000;        // Maximum pulse width from receiver (in microseconds)

void motor_init();

//this atm requires a value form 0-255
void motor_define_speed(int speed);