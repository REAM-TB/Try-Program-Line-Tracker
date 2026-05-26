#include <Arduino.h>
#include "bacaSensor.h"

int sensorADC[JUMLAH_SENSOR];
int sensorADCMid[JUMLAH_SENSOR_MID];
int sensorADCSide[JUMLAH_SENSOR_MID];
int sensorDigitalMid[JUMLAH_SENSOR_MID];
int sensorDigitalSide[JUMLAH_SENSOR_MID];
int treshold = 500;
int sumOnSensor = 0;
int sumOnSensorSide = 0;
int sensorWight = 0;
int sensorWightSide = 0;
int bitsensor = 0;
unsigned int bitsensorSide = 0;
int WeightValue[JUMLAH_SENSOR_MID] = { 10, 20, 30, 40, 50, 60, 70, 80 };
int bitWeight[JUMLAH_SENSOR_MID] = { 1, 2, 4, 8, 16, 32, 64, 128 };
String warnaSensor = "hitam";

static const int sideSensorIndex[JUMLAH_SENSOR_MID] = { 12, 13, 14, 15, 0, 1, 2, 3 };


void initBacaSensor() {
    pinMode(SEL_A, OUTPUT);
    pinMode(SEL_B, OUTPUT);

    analogReadResolution(12);       // 0-4095
    analogSetAttenuation(ADC_11db); // range ~0-3.3V
}

void bacaSensor() {
    digitalWrite(SEL_A, 0); digitalWrite(SEL_B, 0);
    delayMicroseconds(10);
    sensorADC[14] = analogRead(32);
    sensorADC[3]  = analogRead(33);
    sensorADC[6]  = analogRead(34);
    sensorADC[8]  = analogRead(35);

    digitalWrite(SEL_A, 1); digitalWrite(SEL_B, 0);
    delayMicroseconds(10);
    sensorADC[12] = analogRead(32);
    sensorADC[2]  = analogRead(33);
    sensorADC[4]  = analogRead(34);
    sensorADC[9]  = analogRead(35);

    digitalWrite(SEL_A, 0); digitalWrite(SEL_B, 1);
    delayMicroseconds(10);
    sensorADC[13] = analogRead(32);
    sensorADC[0]  = analogRead(33);
    sensorADC[5]  = analogRead(34);
    sensorADC[11] = analogRead(35);

    digitalWrite(SEL_A, 1); digitalWrite(SEL_B, 1);
    delayMicroseconds(10);
    sensorADC[15] = analogRead(32);
    sensorADC[1]  = analogRead(33);
    sensorADC[7]  = analogRead(34);
    sensorADC[10] = analogRead(35);
    
    // Debugging setiap sensor
    for (int i = 0; i < JUMLAH_SENSOR; i++) {
        Serial.print(String(sensorADC[i]) + " ");
    }
    Serial.println();
}


void bacaSensorMid(){
    digitalWrite(SEL_A, 0); digitalWrite(SEL_B, 0);
    delayMicroseconds(10);
    sensorADCMid[2]  = analogRead(34);
    sensorADCMid[4]  = analogRead(35);

    digitalWrite(SEL_A, 1); digitalWrite(SEL_B, 0);
    delayMicroseconds(10);
    sensorADCMid[0]  = analogRead(34);
    sensorADCMid[5]  = analogRead(35);

    digitalWrite(SEL_A, 0); digitalWrite(SEL_B, 1);
    delayMicroseconds(10);
    sensorADCMid[1]  = analogRead(34);
    sensorADCMid[7] = analogRead(35);

    digitalWrite(SEL_A, 1); digitalWrite(SEL_B, 1);
    delayMicroseconds(10);
    sensorADCMid[3]  = analogRead(34);
    sensorADCMid[6] = analogRead(35);
}

void bacaMid() {
    bacaSensorMid();

    sumOnSensor = 0;
    sensorWight = 0;
    bitsensor = 0;
    for(int i = 0; i < JUMLAH_SENSOR_MID; i++) {
        if (warnaSensor == "putih") {
            if (sensorADCMid[i] < treshold) {
                sensorDigitalMid[i] = 1;
            } else {
                sensorDigitalMid[i] = 0;
            }
        } else if (warnaSensor == "hitam") {
            if (sensorADCMid[i] > treshold) {
                sensorDigitalMid[i] = 1;
            } else {
                sensorDigitalMid[i] = 0;
            }
        } else {
            if (sensorADCMid[i] < treshold) {
                sensorDigitalMid[i] = 0;
            } else {
                sensorDigitalMid[i] = 1;
            }
        }
        sumOnSensor += sensorDigitalMid[i];
        sensorWight += sensorDigitalMid[i] * WeightValue[i];
        bitsensor += sensorDigitalMid[i] * bitWeight[7-i];

        Serial.print(sensorDigitalMid[i]);
        // Serial.println(String(bitRead(bitsensor, 7-i)));
    }
        // Serial.print(String(" " + String(bitsensor)));
        // Serial.print(String(" " + String(sumOnSensor)+" " + String(sensorWight)+" " + String(bitsensor)));
        // Serial.println();

}

void displaySensorMid() {
    for (int i = 7; i >= 0; i--) {
        Serial.print(String(bitRead(bitsensor, i)));
    }
}


void bacaSensorSide(){
    digitalWrite(SEL_A, 0); digitalWrite(SEL_B, 0);
    delayMicroseconds(10);
    sensorADCSide[3]  = analogRead(33);
    sensorADCSide[6] = analogRead(32);

    digitalWrite(SEL_A, 1); digitalWrite(SEL_B, 0);
    delayMicroseconds(10);
    sensorADCSide[2]  = analogRead(33);
    sensorADCSide[4] = analogRead(32);

    digitalWrite(SEL_A, 0); digitalWrite(SEL_B, 1);
    delayMicroseconds(10);
    sensorADCSide[0]  = analogRead(33);
    sensorADCSide[5] = analogRead(32);


    digitalWrite(SEL_A, 1); digitalWrite(SEL_B, 1);
    delayMicroseconds(10);
    sensorADCSide[1]  = analogRead(33);
    sensorADCSide[7] = analogRead(32);

    // for (int i = 0; i < JUMLAH_SENSOR_MID; i++) {
    //     Serial.print(String(sensorADCSide[i]) + " ");
    // }
    // Serial.println();
}

void bacaSide() {
    bacaSensorSide();

    sumOnSensorSide = 0;
    sensorWightSide = 0;
    bitsensorSide = 0;

    for (int i = 0; i < JUMLAH_SENSOR_MID; i++) {
        if (warnaSensor == "putih") {
            if (sensorADCSide[i] < treshold) {
                sensorDigitalSide[i] = 1;
            } else {
                sensorDigitalSide[i] = 0;
            }
        } else if (warnaSensor == "hitam") {
            if (sensorADCSide[i] > treshold) {
                sensorDigitalSide[i] = 1;
            } else {
                sensorDigitalSide[i] = 0;
            }
        } else {
            if (sensorADCSide[i] < treshold) {
                sensorDigitalSide[i] = 0;
            } else {
                sensorDigitalSide[i] = 1;
            }
        }

        sumOnSensorSide += sensorDigitalSide[i];
        sensorWightSide += sensorDigitalSide[i] * WeightValue[i];
        bitsensorSide += sensorDigitalSide[i] * bitWeight[7-i];
        Serial.print(sensorDigitalSide[i]);
    }
}

void displaySensorSide() {
    for (int i = 7; i >= 0; i--) {
        Serial.print(String(bitRead(bitsensorSide, i)));
    }
}


void side_callibrate(){
    bacaSide();

    // display sensor data
    for (int i = 8; i >= 0; i--) {
    Serial.print(String(bitRead(bitsensorSide, i)));
    Serial.print(" ");
    }

    if(bitsensor == 0b10000000) {
    Serial.print("Side : Left");
    }else if(bitsensor == 0b00000001) {
    Serial.print("Side : Right");
    } else {
    Serial.print("Side : null");
    }
    Serial.println();
}
