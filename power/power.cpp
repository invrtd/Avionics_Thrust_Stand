#include "power.h"
#include "../config.h"
#include <Arduino.h>

void power_init(){
Serial.println("power_init complete");
}

int sense_voltage(){
  return analogRead(A1);

}

