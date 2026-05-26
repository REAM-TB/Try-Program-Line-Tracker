#pragma once

#define JUMLAH_SENSOR_MID 8
#define JUMLAH_SENSOR 16

#define SEL_A 25
#define SEL_B 26

extern int sensorADC[JUMLAH_SENSOR];

extern int sensorADCMid[JUMLAH_SENSOR_MID];
extern int sensorDigitalMid[JUMLAH_SENSOR_MID];
extern int sensorDigitalSide[JUMLAH_SENSOR_MID];
extern int treshold;
extern int sumOnSensor;
extern int sumOnSensorSide;
extern int sensorWight;
extern int bitsensor;
extern unsigned int bitsensorSide;
extern int WeightValue[JUMLAH_SENSOR_MID];
extern int bitWeight[JUMLAH_SENSOR_MID];
extern String warnaSensor;

void initBacaSensor();
void bacaMid();
void bacaSide();
void bacaSensor();
void bacaSensorSide();
void bacaSensorMid();
void side_callibrate();
void displaySensorMid();
void displaySensorSide();
