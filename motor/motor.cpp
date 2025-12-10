#include <Arduino.h>
#include <Servo.h>
#include "../config.h"


// Pin Definitions
Servo esc;                            // ESC Object
unsigned long lastPulseTime = 0;      // Timestamp of last valid throttle pulse (microseconds)
uint16_t lastPulseWidth = 1500;       // Last valid pulse width (microseconds)
const int RC_MIN_PULSE = 1055;        // Minimum RC pulse (microseconds)
const int RC_MAX_PULSE = 2000;        // Maximum RC pulse (microseconds)
const int THROTTLE_MIN = 0;           // Minimum throttle (0%)
const int THROTTLE_MAX = 255;         // Max throttle (100%)
const int ESC_PIN = 13;               // ESC signal pin (GPI013)
const int ARM_PULSE = 1000;           // Minimum throttle pulse
const int ARM_TIME = 3000;            // ESC arming time (milliseconds)

enum MotorState {
  MOTOR_DISARMED,
  MOTOR_ARMING, 
  MOTOR_READY,
  MOTOR_RUNNING, 
  MOTOR_FAILSAFE
};

MotorState currentMotorState = MOTOR_DISARMED;

struct MotorStatus {
  MotorState state;     // Current motor 
  uint8_t target;       // Commanded 0-255 
  uint8_t current;      // Actualliy applied 0-255
}

MotorStatus motorStatus;

void setup() {
  Serial.begin(9600);
  initESCOutput();      // Initialize ESC Output

  lastPulseTime = micros();
  lastPulseWidth = 1500;
  currentMotorState = MOTOR_DISARMED; 

  Serial.println("System init complete. Motor DISARMED");

}

//
// ESC Output and Throttle Commands
//
void initESCOutput() {
esc.attach(ESC_PIN, 1000, 2000); // Attach ESC signal pin with min/max pulse widths
// Arm ESC at min throttle
escArmingSequence();
}

void setESCthrottle(uint8_t throttle) {
  // throttle = 0-255 -> 1000-2000 us
  uint16_t pwmSignal = 1000 + ((uint32_t)throttle * 1000) / 255;
  esc.writeMicroseconds(pwmSignal); // Send PWM signal to ESC
}

//
// ESC Arming and Setup Sequence
//
void escArmingSequence() {
  Serial.println("Arming ESC...");
  esc.writeMicroseconds(ARM_PULSE); // Send minimum throttle
  delay(ARM_TIME);
  Serial.println("ESC Armed.");
}

//
// Reading Throttle Input
//
int16_t readThrottleInput() {
  // Measure pulse width from receiver (in microseconds)
  uint32_t pulseWidth = pulseIn(MOTOR_PWM_PIN, HIGH, 25000); // Timeout after 25 ms

  // If pulseIn times out, then pulse = 0 and signal is lost
  if (pulseWidth == 0) {
    Serial.println("Throttle signal lost!");
    return -1;                                                // Indicate signal loss
  }

  // Constrain pulse width to expected range (1000-2000us)
  pulseWidth = constrain(pulseWidth, RC_MIN_PULSE, RC_MAX_PULSE);                 // MAYBE ADJUST BASED ON RECEIVER SPECS

  // Map pulse width -> 0-255 throttle 
  uint32_t numer = (pulseWidth - RC_MIN_PULSE) * 255UL;
  uint32_t denom = (RC_MAX_PULSE - RC_MIN_PULSE);
  uint8_t throttle =  numer / denom;

  // Store last valid throttle for failsafe
  lastPulseTime = micros();
  lastPulseWidth = pulseWidth;
  return (int16_t)throttle;
}

//
// Safety and Failsafe Mechanisms
//
bool checkSignalLoss() {
  unsigned long currentTime = micros();
  // If more than 30 ms since last valid pulse, consider signal lost
  if ((currentTime - lastPulseTime) > 30000UL) {  
    return false;   // Signal lost
  }
return true;        // Signal is valid
}


void failSafeProcedure() {
  if (currentMotorState == MOTOR_FAILSAFE) {
    setESCthrottle(0);        // Already in failsafe mode, do nothing
    return;
  }
  
  // Shut down motor 
  setESCthrottle(0);

  // Enter FAILSAFE state to prevent further operation
  currentMotorState = MOTOR_FAILSAFE;

  // Log failsafe event
  Serial.println("Entering FAILSAFE mode: motor stopped");
}

//
// Motor State Machine
//
void motorStateMachine(uint8_t cmd) {
  switch(currentMotorState) {

    case MOTOR_DISARMED:
      // Start up label after ESC is armed 
      break;

    case MOTOR_ARMING:
      // Run the ESC arming sequence
      escArmingSequence(); // After arming, transition to READY state
      currentMotorState = MOTOR_READY;
      break;
    
      case MOTOR_READY:
      if (cmd > 12) {    // Small deadband to avoid accidental starts
        currentMotorState = MOTOR_RUNNING;
      }
      break;
    
    case MOTOR_RUNNING:
      if (cmd < 5) {    // Small deadband to avoid accidental stops
        currentMotorState = MOTOR_READY;
      }
      break;
    
    case MOTOR_FAILSAFE:
      // Remain in failsafe until reset
      setESCthrottle(0);
      break;
  }
}

/// NMEA Motor Sentence
void sendMotorStatusNMEA() {
  String sentence = build_nmea(
    "MOTOR,STATE",
    (int)currentMotorState,
    motorStatus.target,
    motorStatus.current,
    lastPulseWidth
  );

  Serial.print(sentence);

}





