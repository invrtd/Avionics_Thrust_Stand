#include "config.h"
#include "util.h"
#include "rpm/rpm.h"
#include "thrust/thrust.h"
#include "motor/motor.h"
#include "airspeed/airspeed.h"
#include "power/power.h"

void setup() {
  rpm_init();
  thrust_init();
  motor_init();
  airspeed_init();
  power_init();
}

void loop() {
  rpm_update();
  thrust_update();
  airspeed_update();
  power_update();
  motor_update();   

}
