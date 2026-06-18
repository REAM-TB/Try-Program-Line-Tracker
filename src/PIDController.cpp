#include <Arduino.h>
#include "bacaSensor.h"
#include "sensorJarak.h"
#include "PIDController.h"
#include "motorControl.h"
#include "communicationrxtx.h"
    
float line_position = 0.0;
float error = 0.0;
float center_position = 45.0;
float right_motor_correction, left_motor_correction;
float derivative, previous_error;
static bool rightTurnSequenceDone = false;
int countStandBy = 0;
float jarakCM = 0.0;

void resetPIDState() {
    line_position = 0.0;
    error = 0.0;
    right_motor_correction = 0.0;
    left_motor_correction = 0.0;
    derivative = 0.0;
    previous_error = 0.0;
}

void PID(float base_speed, float Kp, float Kd) {
    float jarakTerbaca = bacaJarakCM();
    if (jarakTerbaca > 0) {
        jarakCM = jarakTerbaca;
    }
    bacaMid();

    if (bitsensor != 0b11111111) {
        rightTurnSequenceDone = false;
    }

    if (bitsensor == 0b11111111) {
        displaySensorMid();

        if (!rightTurnSequenceDone) {
            countStandBy++;

            if (countStandBy == 1 && jarakCM > 0 && jarakCM < 9) {
                Serial.println(" State Pickup, Jarak :" + String(jarakCM) + " cm");
                motorStop();
                clearReceivedSelesai();
                sendCommunicationRxTxMessage("pickup");
                rightTurnSequenceDone = true;

                Serial.println("Menunggu balasan selesai...");
                while (!hasReceivedSelesai()) {
                    delay(10);
                }

                Serial.println("Balasan selesai diterima");
                delay(2000);
                setMotorSpeed(-getKecepatanBalik(), -getKecepatanBalik());
                delay(1000);
                setMotorSpeed(-getKecepatanBalik(), getKecepatanBalik());
                delay(getWaktuBalik());
                while (true) {
                    setMotorSpeed(-getKecepatanBalik(), getKecepatanBalik());
                    bacaMid();

                    if (sensorWight == getOrientasiBalik() && sensorWight != 0 && sensorWight != 60 && sensorWight != 30 && sensorWight != 100 && sensorWight != 10) { // line found
                        motorStop();
                        resetPIDState();
                        Serial.println("LINE FOUND - CONTINUE PID");
                        return;
                    }
                }
            }

            else if (countStandBy >= 2) {
                Serial.println("parkir");
                setMotorSpeed(-getKecepatanBalik(), getKecepatanBalik()); // belok kanan
                delay(getWaktuparkir());

                while (true) {
                    setMotorSpeed(-getKecepatanBalik(), getKecepatanBalik()); // belok kanan
                    bacaMid();

                    if (sensorWight == getOrientasiParkir() && sensorWight != 0) { // line found'
                        while(true) {
                            setMotorSpeed(-getKecepatanParkir(), -getKecepatanParkir()); // mundur
                            bacaSide(); // update sensorDigitalSide

                            if (sensorDigitalSide[0] == 1 || sensorDigitalSide[7] == 1) {
                                pidEnabled = false;
                                startupDone = false;
                                motorStop();
                                Serial.println("PID OFF - COUNT STANDBY 2");
                                Serial.println(String(sensorDigitalSide[0]) + " " + String(sensorDigitalSide[7]));
                                return;
                            }
                        }
                    }
                }
            }
        } else {
            motorStop();
        }

        Serial.print(String(" Weight :" + String(sensorWight)));
        Serial.print(" Error :0");
        Serial.print(" Correction speed :turn right");
        Serial.println();
        return;
    }

    if (sumOnSensor > 0) {
        line_position = sensorWight / sumOnSensor;
        error = center_position - line_position;
    } else {
        // LOST LINE
        if (previous_error > 0) {
            error = -40;   // belok kanan
        } else {
            error = +40;  // belok kiri
        }
    }


    right_motor_correction = base_speed - (Kp * error +  Kd * (error - previous_error));
    left_motor_correction = base_speed + (Kp * error + Kd * (error - previous_error));

    if (right_motor_correction > maxSpeed) right_motor_correction = maxSpeed;
    if (left_motor_correction > maxSpeed) left_motor_correction = maxSpeed;

    previous_error = error;

    setMotorSpeed(left_motor_correction, right_motor_correction);

    // display sensor data
    displaySensorMid();

    Serial.print(String(" Weight :" + String(sensorWight)));
    Serial.print(" Error :" + String(error));
    Serial.print(" Correction speed :" + String(left_motor_correction) + ", " + String(right_motor_correction));
    Serial.print(" Jarak :" + String(jarakCM) + " cm");
    Serial.println();
    

}