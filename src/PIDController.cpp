#include <Arduino.h>
#include "bacaSensor.h"
#include "PIDController.h"
#include "motorControl.h"
    
float line_position = 0.0;
float error = 0.0;
float center_position = 45.0;
float right_motor_correction, left_motor_correction;
float derivative, previous_error;
static bool rightTurnSequenceDone = false;

void resetPIDState() {
    line_position = 0.0;
    error = 0.0;
    right_motor_correction = 0.0;
    left_motor_correction = 0.0;
    derivative = 0.0;
    previous_error = 0.0;
}

void PID(float base_speed, float Kp, float Kd) {
    bacaMid();

    if (bitsensor != 0b11111111) {
        rightTurnSequenceDone = false;
    }

    if (bitsensor == 0b11111111) {
        displaySensorMid();

        if (!rightTurnSequenceDone) {
            motorStop();
            delay(2000);
            setMotorSpeed(-base_speed, base_speed); // Belok kanan
            delay(950);
            motorStop();
            rightTurnSequenceDone = true;
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
    // displaySensorSide();

    Serial.print(String(" Weight :" + String(sensorWight)));
    Serial.print(" Error :" + String(error));
    Serial.print(" Correction speed :" + String(left_motor_correction) + ", " + String(right_motor_correction));
    Serial.println();

}