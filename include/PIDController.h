#pragma once

extern int maxSpeed;
extern int countStandBy;
extern bool pidEnabled;
extern bool startupDone;

void PID(float base_speed, float Kp, float Kd);
void resetPIDState();
