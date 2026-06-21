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

const int START_HOUR = 8;
const int START_MINUTE = 55;

bool scheduleAlreadyTriggered = false;

uint8_t LFRobotMac[] = {0x14, 0x2B, 0x2F, 0xC6, 0xFC, 0x80};

bool receivedFinish = false;
bool waitingFinish = false;
bool goAlreadySent = false;

unsigned long goSentTime = 0;
const unsigned long FINISH_TIMEOUT = 60000;

int base_speed = 160;
int maxSpeed = 180;

float Kp = 8.0;
float Kd = 1.3;

int waktu_balik = 800;
int orientasi_balik = 50;
int kecepatan_balik = 160;

int waktu_parkir = 600;
int orientasi_parkir = 70;
int kecepatan_parkir = 160;

bool pidEnabled = false;
bool startupDone = false;

const float jarakAmbangBatasCM = 20.0;

extern int countStandBy;

// ================= RTC STATUS =================
bool rtcReadyPrinted = false;

// ================= KIRIM GO RETRY =================
bool kirimGo() {
  for (int i = 0; i < 5; i++) {
    Serial.print("Mengirim GO percobaan ke-");
    Serial.println(i + 1);

    if (espNow.send("go")) {
      Serial.println("GO berhasil dikirim");
      return true;
    }

    Serial.println("GO gagal, retry...");
    delay(200);
  }

  Serial.println("GO gagal total");
  return false;
}

// ================= AMBIL WAKTU DARI WIFI =================
void syncRTCOnce() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Menghubungkan WiFi untuk sinkron waktu...");

  unsigned long startWifi = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startWifi < 10000) {
    delay(100);
  }

  rtc.begin();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");

    unsigned long startRTC = millis();
    while (!rtc.isReady() && millis() - startRTC < 5000) {
      rtc.update();
      delay(100);
    }

    if (rtc.isReady()) {
      Serial.print("RTC Ready: ");
      Serial.println(rtc.getDateTime());
      rtcReadyPrinted = true;
    } else {
      Serial.println("RTC belum ready, lanjut tanpa auto time.");
    }
  } else {
    Serial.println("WiFi gagal connect, lanjut tanpa auto time.");
  }

  WiFi.disconnect(true);
  delay(300);
  WiFi.mode(WIFI_STA);

  Serial.println("WiFi router diputus. ESP-NOW akan digunakan.");
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  inisialisasibuzzer();
  MAC_Cheker();

  initBacaSensor();
  initSensorJarak();
  initMotorControl();
  initButton();
  motorStop();

  // Ambil waktu sekali, lalu putus WiFi
  syncRTCOnce();

  // Mulai ESP-NOW setelah WiFi router diputus
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
      waitingFinish = false;
      goAlreadySent = false;

      Serial.println("FINISH diterima dari pickup robot");

      // Setelah pickup selesai, PID boleh lanjut lagi
      pidEnabled = true;
      startupDone = false;
      countStandBy = 0;
      resetPIDState();
    }
  });

  espNow.onSend([](bool success) {
    Serial.println(success ? "ESP-NOW terkirim" : "ESP-NOW gagal kirim");
  });

  Serial.println("System ready.");
  Serial.println("Tombol start tetap bisa digunakan.");
}

// ================= LOOP =================
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

  // ================= TUNGGU FINISH =================
  if (waitingFinish) {
    motorStop();

    if (receivedFinish) {
      waitingFinish = false;
      goAlreadySent = false;
      receivedFinish = false;

      Serial.println("Pickup selesai, lanjut PID");
    }

    if (millis() - goSentTime > FINISH_TIMEOUT) {
      waitingFinish = false;
      goAlreadySent = false;

      Serial.println("Timeout menunggu finish. Sistem lanjut.");
      pidEnabled = true;
      resetPIDState();
    }

    return;
  }

  // ================= PID CONTROL =================
  if (pidEnabled) {
    PID(base_speed, Kp, Kd);
  } else {
    motorStop();
  }

  /*
    CATATAN:
    Kalau di dalam PID atau logic sensor kamu ada kondisi robot sudah standby/parkir,
    panggil bagian ini:

    if (!goAlreadySent) {
      pidEnabled = false;
      motorStop();

      receivedFinish = false;

      if (kirimGo()) {
        goAlreadySent = true;
        waitingFinish = true;
        goSentTime = millis();
        Serial.println("Menunggu balasan finish...");
      } else {
        Serial.println("Gagal kirim GO ke pickup robot");
        pidEnabled = true;
      }
    }
  */
}

// ================= GETTER =================
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