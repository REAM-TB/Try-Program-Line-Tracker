#include <Arduino.h>
#include "bacaSensor.h"
#include "PIDController.h"
#include "motorControl.h"

int base_speed = 140
;
float Kp = 9.0;
float Kd = 0.8;

const int BUTTON_PIN = 27;
const unsigned long DEBOUNCE_DELAY = 50;

bool pidEnabled = false;
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;


void setup() {
  Serial.begin(115200);
  initBacaSensor();
  initMotorControl();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  motorStop();
}

void loop() {
  bool buttonReading = digitalRead(BUTTON_PIN);

  if (buttonReading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY && buttonReading != stableButtonState) {
    stableButtonState = buttonReading;

    if (stableButtonState == LOW) {
      pidEnabled = !pidEnabled;

      if (pidEnabled) {
        resetPIDState();
        Serial.println("PID ON");
      } else {
        motorStop();
        Serial.println("PID OFF");
      }
    }
  }

  lastButtonReading = buttonReading;

  if (pidEnabled) {
    PID(base_speed, Kp, Kd);
  } else {
    motorStop();
  }
}