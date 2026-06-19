#ifndef SIMPLE_ESPNOW_H
#define SIMPLE_ESPNOW_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

typedef void (*ReceiveCallback)(String message);
typedef void (*SendCallback)(bool success);

class SimpleEspNow {
public:
  bool begin(uint8_t channel = 6);

  bool setPeer(const uint8_t mac[6]);
  bool send(String message);

  void onReceive(ReceiveCallback callback);
  void onSend(SendCallback callback);

private:
  static ReceiveCallback receiveCallback;
  static SendCallback sendCallback;

  static uint8_t peerMac[6];
  static bool peerReady;

  static void receiveHandler(const uint8_t *mac, const uint8_t *data, int len);
  static void sendHandler(const uint8_t *mac, esp_now_send_status_t status);
};

#endif