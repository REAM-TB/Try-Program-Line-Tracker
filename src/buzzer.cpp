#include <Arduino.h>
#include "buzzer.h"

static const int buzzerPin = 15;

void inisialisasibuzzer() {
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  for (int i = 0; i < 2; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);

    if (i < 1) {
      delay(1000);
    }
  }
  digitalWrite(buzzerPin, LOW);
}
