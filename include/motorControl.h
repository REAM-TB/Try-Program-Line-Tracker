#pragma once

// Motor A (Left)
#define LEFT_EN 13
#define LEFT_FORWARD 2
#define LEFT_BACKWARD 4

// Motor B (Right)
#define RIGHT_EN 5
#define RIGHT_FORWARD 17
#define RIGHT_BACKWARD 16

// PWM Configuration
#define PWM_FREQ_LEFT 3300
#define PWM_FREQ_RIGHT 2220
#define PWM_RESOLUTION 8
#define PWM_MAX 255

// PWM Channels
#define CH_LEFT 0
#define CH_RIGHT 1

void initMotorControl();
void motorForward(float speedLeft, float speedRight);
void motorBackward(float speedLeft, float speedRight);
void motorTurnLeft(float speedLeft, float speedRight);
void motorTurnRight(float speedLeft, float speedRight);
void motorStop();
void setMotorSpeed(float leftSpeed, float rightSpeed);
