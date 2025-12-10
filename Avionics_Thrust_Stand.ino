#include "config.h"
#include "util.h"
#include "rpm/rpm.h"
#include "thrust/thrust.h"
#include "motor/motor.h"
#include "airspeed/airspeed.h"
#include "power/power.h"
#include "power/power.cpp"
void setup() {
  power_init();
  Serial.begin(9600);
}

void loop() {
  delay(200);
  power_update();
  power_output();
}
