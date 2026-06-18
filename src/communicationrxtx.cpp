#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "communicationrxtx.h"

static uint8_t receiverAddress[] = {0xCC, 0xBA, 0x97, 0x27, 0x55, 0x98};

static const char pickupMessage[] = "pickup";
static volatile bool receivedSelesai = false;

static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Status Kirim: ");

  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Berhasil");
  } else {
    Serial.println("Gagal");
  }
}

static void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  char msg[32];
  int copyLen = len;

  if (copyLen > (int)sizeof(msg) - 1) {
    copyLen = sizeof(msg) - 1;
  }

  memcpy(msg, incomingData, copyLen);
  msg[copyLen] = '\0';

  if (strcmp(msg, "selesai") == 0) {
    receivedSelesai = true;
    Serial.println("selesai");
  } else {
    Serial.print("Pesan diterima: ");
    Serial.println(msg);
  }
}

void initCommunicationRxTx() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW gagal");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Tambah peer ESP-NOW gagal");
    return;
  }

  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP-NOW siap kirim dan terima");
}

void sendCommunicationRxTxMessage(const char *message) {
  esp_err_t result = esp_now_send(receiverAddress, (const uint8_t *)message, strlen(message) + 1);

  if (result == ESP_OK) {
    Serial.print("Mengirim: ");
    Serial.println(message);
  } else {
    Serial.println("Gagal mengirim pesan");
  }
}

bool hasReceivedSelesai() {
  return receivedSelesai;
}

void clearReceivedSelesai() {
  receivedSelesai = false;
}

void handleCommunicationRxTx() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equalsIgnoreCase("pickup")) {
      sendCommunicationRxTxMessage(pickupMessage);
    }
  }
}