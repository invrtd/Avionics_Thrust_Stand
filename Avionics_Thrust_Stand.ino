#include <Arduino.h>
#include "includes.h"
#include "config.h"
#include "util.h"

#include "rpm/rpm.h"
#include "rpm/rpm.cpp"

#include "thrust/thrust.h"
#include "thrust/thrust.cpp"

#include "motor/motor.h"
#include "motor/motor.cpp"

#include "airspeed/airspeed.h"
#include "airspeed/airspeed.cpp"

#include "power/power.h"
#include "power/power.cpp"


// look at util.h file
void setup() {
  Serial.begin(9600);
  rpm_init();
  thrust_init();
  motor_init();
  airspeed_init();
  power_init();
  Serial.begin(9600);
}

void loop() {
  /*rpm_update();
  thrust_update();
  airspeed_update();
  power_update();
  motor_update();   

  build_nmea_sentence();
8*/
  delay(1000);
  
  Serial.println(build_nmea("pws", 12.34, 5.67, 70.89));
}
