#include <Arduino.h>
#include "button.h"

const int BUTTON_PIN = 27;
const unsigned long DEBOUNCE_DELAY = 50;

bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;

void initButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

bool updateButtonToggle(bool currentState) {
  bool buttonReading = digitalRead(BUTTON_PIN);

  if (buttonReading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY && buttonReading != stableButtonState) {
    stableButtonState = buttonReading;

    if (stableButtonState == LOW) {
      currentState = !currentState;
    }
  }

  lastButtonReading = buttonReading;
  return currentState;
}