#include <Arduino.h>
#include "power.h"
#include "../config.h"

void motor_define_speed(int speed){
  analogWrite(MOTOR_PWD_PIN, speed)
}