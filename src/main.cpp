#include <Arduino.h>
#include <WiFi.h>

#include "bacaSensor.h"
#include "button.h"
#include "buzzer.h"
#include "PIDController.h"
#include "motorControl.h"
#include "sensorJarak.h"
#include "MAC_Cheker.h"
#include "SimpleEspNow.h"
#include "RealTimeClk.h"

SimpleEspNow espNow;
RealtimeClock rtc;

const char* WIFI_SSID = "Erick";
const char* WIFI_PASS = "bdkey354";

const int START_HOUR = 11;
const int START_MINUTE = 17;

bool scheduleAlreadyTriggered = false;

uint8_t LFRobotMac[] = {0x14, 0x2B, 0x2F, 0xC6, 0xFC, 0x80};

bool receivedFinish = false;

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

// ================= WIFI RTC NON-BLOCKING =================
unsigned long lastWifiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 1000;

bool rtcReadyPrinted = false;

void setup() {
  Serial.begin(115200);

  inisialisasibuzzer();
  MAC_Cheker();

  initBacaSensor();
  initSensorJarak();
  initMotorControl();
  initButton();
  motorStop();

  if (!espNow.begin(6)) {
    Serial.println("SimpleEspNow gagal mulai");
  } else {
    Serial.println("SimpleEspNow aktif");
  }

  if (!espNow.setPeer(LFRobotMac)) {
    Serial.println("Peer pickup robot gagal ditambahkan");
  } else {
    Serial.println("Peer pickup robot siap");
  }

  espNow.onReceive([](String message) {
    message.trim();
    message.toLowerCase();

    Serial.print("ESP-NOW masuk: ");
    Serial.println(message);

    if (message == "finish") {
      receivedFinish = true;
    }
  });

  espNow.onSend([](bool success) {
    Serial.println(success ? "ESP-NOW terkirim" : "ESP-NOW gagal kirim");
  });

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  rtc.begin();

  Serial.println("WiFi connecting in background...");
  Serial.println("Tombol start tetap bisa digunakan.");
}

void loop() {
  // ================= BUTTON START/STOP =================
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

  // ================= WIFI + RTC NON-BLOCKING =================
  if (millis() - lastWifiCheck >= WIFI_CHECK_INTERVAL) {
    lastWifiCheck = millis();

    if (WiFi.status() == WL_CONNECTED) {
      rtc.update();

      if (rtc.isReady() && !rtcReadyPrinted) {
        rtcReadyPrinted = true;
        Serial.print("RTC Ready: ");
        Serial.println(rtc.getDateTime());
      }
    } else {
      Serial.println("WiFi belum connect, tombol tetap aktif.");
    }
  }

  // ================= AUTO START BY TIME =================
  if (rtc.isReady()) {
    int hourNow = rtc.getHour();
    int minuteNow = rtc.getMinute();
    int secondNow = rtc.getSecond();

    if (hourNow == START_HOUR &&
        minuteNow == START_MINUTE &&
        secondNow >= 0 &&
        secondNow <= 10 &&
        !scheduleAlreadyTriggered) {

      pidEnabled = true;
      startupDone = false;
      countStandBy = 0;
      resetPIDState();

      scheduleAlreadyTriggered = true;

      Serial.println("AUTO START BY TIME");
    }

    if (minuteNow != START_MINUTE) {
      scheduleAlreadyTriggered = false;
    }
  }

  // ================= PID CONTROL =================
  if (pidEnabled) {
    PID(base_speed, Kp, Kd);
  } else {
    motorStop();
  }
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