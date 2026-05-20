#include <Arduino.h>
#include "bacaSensor.h"
#include "button.h"
#include "PIDController.h"
#include "motorControl.h"

int base_speed = 140
;
float Kp = 9.0;
float Kd = 0.8;

bool pidEnabled = false;


void setup() {
  Serial.begin(115200);
  initBacaSensor();
  initMotorControl();
  initButton();
  motorStop();
}

void loop() {
  bool updatedPidEnabled = updateButtonToggle(pidEnabled);

  if (updatedPidEnabled != pidEnabled) {
    pidEnabled = updatedPidEnabled;

    if (pidEnabled) {
      resetPIDState();
      Serial.println("PID ON");
    } else {
      motorStop();
      Serial.println("PID OFF");
    }
  }

  if (pidEnabled) {
    PID(base_speed, Kp, Kd);
  } else {
    motorStop();
  }
}