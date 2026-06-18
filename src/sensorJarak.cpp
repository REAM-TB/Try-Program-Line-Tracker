#include <Arduino.h>

const int trigPin = 14;
const int echoPin = 12;

void initSensorJarak() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

float bacaJarakCM() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long durasi = pulseIn(echoPin, HIGH, 30000);
    if (durasi == 0) {
        return -1.0;
    }

    return durasi * 0.0343f / 2.0f;
}