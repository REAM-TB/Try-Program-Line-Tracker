#include <Arduino.h>

const int trigPin = 14;
const int echoPin = 12;

void initSensorJarak() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
}

static float readJarakSample() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long durasi = pulseIn(echoPin, HIGH, 30000);
    if (durasi == 0) {
        return -1.0f;
    }

    return durasi * 0.0343f / 2.0f;
}

float bacaJarakCM() {
    float samples[3];

    samples[0] = readJarakSample();
    delay(10);
    samples[1] = readJarakSample();
    delay(10);
    samples[2] = readJarakSample();

    if (samples[0] > samples[1]) {
        float temp = samples[0];
        samples[0] = samples[1];
        samples[1] = temp;
    }

    if (samples[1] > samples[2]) {
        float temp = samples[1];
        samples[1] = samples[2];
        samples[2] = temp;
    }

    if (samples[0] > samples[1]) {
        float temp = samples[0];
        samples[0] = samples[1];
        samples[1] = temp;
    }

    return samples[1];
}