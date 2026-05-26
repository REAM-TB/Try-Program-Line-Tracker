#include <Arduino.h>
#include "bacaSensor.h"
#include "button.h"
#include "buzzer.h"
#include "PIDController.h"
#include "motorControl.h"
#include "bacaSensor.h"

int base_speed = 160;
int maxSpeed = 180;

float Kp = 9.0;
float Kd = 0.8;


bool pidEnabled = false;
bool startupDone = false;


void setup() {
  Serial.begin(115200);
  inisialisasibuzzer();  
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
      startupDone = false;
      resetPIDState();
      Serial.println("PID ON");
    } else {
      motorStop();
      startupDone = false;
      Serial.println("PID OFF");
    }
  }

  if (pidEnabled) {
    // if (!startupDone) {
    //   motorForward(base_speed, base_speed);
    //   delay(1000);
    //   motorStop();
    //   startupDone = true;
    //   resetPIDState();
    // }
    PID(base_speed, Kp, Kd);
  } else {
    motorStop();
  }
  
}