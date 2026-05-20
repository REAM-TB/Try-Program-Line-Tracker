#include <Arduino.h>
#include "bacaSensor.h"
#include "PIDController.h"
#include "motorControl.h"
    
float line_position = 0.0;
float error = 0.0;
float center_position = 45.0;
float right_motor_correction, left_motor_correction;
float derivative, previous_error;
const int MAX_MOTOR_PWM = 255;

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

    if(right_motor_correction > 255) right_motor_correction = MAX_MOTOR_PWM;
    if(left_motor_correction > 255) left_motor_correction = MAX_MOTOR_PWM;

    previous_error = error;

    setMotorSpeed(left_motor_correction, right_motor_correction);

    // display sensor data
    for (int i = 7; i >= 0; i--) {
        Serial.print(String(bitRead(bitsensor, i)));
    }

    Serial.print(String(" Weight :" + String(sensorWight)));
    Serial.print(" Error :" + String(error));
    Serial.print(" Correction speed :" + String(left_motor_correction) + ", " + String(right_motor_correction));
    Serial.println();

}