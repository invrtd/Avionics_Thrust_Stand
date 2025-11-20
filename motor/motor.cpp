#include <Arduino.h>
#include "power.h"
#include <Servo.h>
#include "../config.h"

void motor_define_speed(int speed){
  analogWrite(MOTOR_PWD_PIN, speed)
}

\\ Pin Definitions
Servo esc;                            \\ ESC Object
const int THROTTLE_PIN = 2;           \\ Reciever Input
const int ESC_PIN = 3;                \\ ESC Signal Output
const int BATTERY_VOLTAGE_PIN = A0;   \\ Battery Voltage Monitor
const int RC_MIN_PULSE = 1000;        \\ Minimum pulse width from receiver (in microseconds)
const int RC_MAX_PULSE = 2000;        \\ Maximum pulse width from receiver (in microseconds)

unsigned long lastPulseTime = 0;      \\ Timestamp of last valid throttle pulse
unsigned long LastPulseWidth = 1500;  \\ Last valid pulse width

enum MotorState {
  MOTOR_DISARMED,
  MOTOR_ARMING, 
  MOTOR_READY,
  MOTOR_RUNNING, 
  MOTOR_FAILSAFE
};

MotorState currentMotorState = MOTOR_DISARMED;

void setup() {
  Serial.begin(9600);
  initESCOutput();
  initThrottleInput();
  escArmingSequence();

  motorState = MOTOR_DISARMED;

  \\ Arming the ESC
  escArmingSequence();

  \\ After arming sequence, set motor state to READY
  currentMotorState = MOTOR_READY;
}

\\
\\ ESC Output and Throttle Commands
\\
void initESCOutput() {
  esc.attach(ESC_PIN);          \\ Attach ESC to pin
  esc.writeMicroseconds(1000);  \\ Set to minimum throttle
  delay(100);                   \\ Safety delay
}

void setESCthrottle(float throttle) {
  \\ Safety mechanism to prevent <0% or >100% throttle
  if (throttle < 0.0f) throttle = 0.0f;
  if (throttle > 1.0f) throttle = 1.0f;

  \\ Convert throttle to PWM signal (1000-2000us)
  int pwmSignal = 1000 + (int)(throttle * 1000.0f);   \\ 1000us = 0%, 2000us = 100%

  esc.writeMicroseconds(pwmSignal); \\ Send PWM signal to ESC
}

\\
\\ ESC Arming and Setup Sequence
\\
void escArmingSequence() {
  Serial.println("Starting ESC Arming Sequence...");

  setESCthrottle(0.0f);                               \\ Set to minimum throttle (1000 us), idle signal

  \\ Hold minimum throttle for ESC arming duration
  unsigned long startTime = millis();
  while (millis() - startTime < 4000) {               \\ 4 seconds ESC arming time
    setESCThrottle(0.0f);
    delay(20);                                        \\ Repeatedly send signal at 50 Hz to ensure reliable arming
  }

  Serial.println("ESC Arming Sequence Complete.");
}

\\
\\ Reading Throttle Input
\\
void initThrottleInput() {
  pinMode(THROTTLE_PIN, INPUT);                       \\ Set throttle pin as input
}

float readThrottleInput() {
  \\ Measure pulse width from receiver (in microseconds)
  unsigned long pulseWidth = pulseIn(THROTTLE_PIN, HIGH, 25000); \\ Timeout after 25ms

  \\ If pulseIn times out, then pulse = 0 and signal is lost
  if pulseWidth == 0 {
    Serial.println("Throttle signal lost!");
    return -1.0f;                                                \\ Indicate signal loss
  }

  \\ Constrain pulse width to expected range (1000-2000us)
  pulseWidth = constrain(pulseWidth, 1000, 2000);                 \\ MAYBE ADJUST BASED ON RECEIVER SPECS

  \\ Map pulse width to throttle percentage (0.0 to 1.0)
  float throttle = (float)(pulseWidth - RC_MIN_PULSE) / float(RC_MAX_PULSE - RC_MIN_PULSE);

  \\ Store last valid throttle for failsafe
  lastPulseTime = micros();
  LastPulseWidth = pulseWidth;
  return throttle;
}

\\
\\ Safety and Failsafe Mechanisms
\\
bool checkSignalLoss() {
  unsigned long currentTime = micros();
  \\ If more than 30 ms since last valid pulse, consider signal lost
  if ((currentTime - lastPulseTime) > 30000UL) {  
    return false;   \\ Signal lost
  }
return true;        \\ Signal is valid
}

checkBatteryVoltage() {             \\ ADJUST LATER

}

failSafeProcedure() {
  \\ Shut down motor 
  setESCthrottle(0.0f);

  \\ Enter FAILSAFE state to prevent further operation
  currentMotorState = MOTOR_FAILSAFE;

  \\ Log failsafe event
  Serial.println("Entering FAILSAFE mode: motor stopped");
}

\\
\\ Motor State Machine
\\
void motorStateMachine(float cmd) {
  switch(motorState) {

    case MOTOR_DISARMED:
      \\ Start up label after ESC is armed 
      break;

    case MOTOR_ARMING:
      \\ Run the ESC arming sequence
      escArmingSequence(); \\ After arming, transition to READY state
      motorState = MOTOR_READY;
      break;
    
      case MOTOR_READY:
      if (cmd > 0.05f) {    \\ Small deadband to avoid accidental starts
        motorState = MOTOR_RUNNING;
      }
      break;
    
    case MOTOR_RUNNING:
      if (cmd < 0.02f) {    \\ Small deadband to avoid accidental stops
        motorState = MOTOR_READY;
      }
      break;
    
    case MOTOR_FAILSAFE:
      \\ Remain in failsafe until reset
      setESCthrottle(0.0f);
      break;
  }
}




