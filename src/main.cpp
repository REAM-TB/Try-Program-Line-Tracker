#include <Arduino.h>
#include "bacaSensor.h"
#include "button.h"
#include "buzzer.h"
#include "PIDController.h"
#include "motorControl.h"
#include "bacaSensor.h"
#include "sensorJarak.h"
#include "MAC_Cheker.h"
#include "communicationrxtx.h"

int base_speed = 160;
int maxSpeed = 180;

float Kp = 9.0;
float Kd = 0.8;

int waktu_balik = 800;
int orientasi_balik = 50;
int kecepatan_balik = 150;

int waktu_parkir = 600;
int orientasi_parkir = 70;
int kecepatan_parkir = 145;

bool pidEnabled = false;
bool startupDone = false;

const float jarakAmbangBatasCM = 20.0;

extern int countStandBy;


void setup() {
  Serial.begin(115200);
  inisialisasibuzzer();  
  MAC_Cheker();
  initCommunicationRxTx();
  initBacaSensor();
  initSensorJarak();
  initMotorControl();
  initButton();
  motorStop();
}

void loop() {
  bool updatedPidEnabled = updateButtonToggle(pidEnabled);

  if (updatedPidEnabled != pidEnabled) {
    pidEnabled = updatedPidEnabled;

    if (pidEnabled) {
      startupDone = false;
      countStandBy = 0;
      resetPIDState();
      Serial.println("PID ON");
    } else {
      motorStop();
      startupDone = false;
      Serial.println("PID OFF");
    }
  }

  
  if (pidEnabled) {
    // float jarakCM = bacaJarakCM();

    // if (jarakCM > 0) {
    //   Serial.print("Distance: ");
    //   Serial.print(jarakCM);
    //   Serial.println(" cm");
    // }
    PID(base_speed, Kp, Kd);
  } else {
    motorStop();
  }

  handleCommunicationRxTx();
  
}

int getWaktuBalik() {
  return waktu_balik;
}

int getWaktuparkir() {
  return waktu_parkir;
}

int getOrientasiBalik() {
  return orientasi_balik;
}

int getOrientasiParkir() {
  return orientasi_parkir;
}

int getKecepatanParkir() {
  return kecepatan_parkir;
}

int getKecepatanBalik() {
  return kecepatan_balik;
}