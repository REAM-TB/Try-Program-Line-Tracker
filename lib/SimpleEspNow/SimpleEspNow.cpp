#include "SimpleEspNow.h"

ReceiveCallback SimpleEspNow::receiveCallback = nullptr;
SendCallback SimpleEspNow::sendCallback = nullptr;

uint8_t SimpleEspNow::peerMac[6] = {0};
bool SimpleEspNow::peerReady = false;

bool SimpleEspNow::begin(uint8_t channel) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    return false;
  }

  esp_now_register_recv_cb(SimpleEspNow::receiveHandler);
  esp_now_register_send_cb(SimpleEspNow::sendHandler);

  return true;
}

bool SimpleEspNow::setPeer(const uint8_t mac[6]) {
  memcpy(peerMac, mac, 6);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_is_peer_exist(peerMac)) {
    esp_now_del_peer(peerMac);
  }

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    peerReady = false;
    return false;
  }

  peerReady = true;
  return true;
}

bool SimpleEspNow::send(String message) {
  if (!peerReady) return false;

  esp_err_t result = esp_now_send(
    peerMac,
    (uint8_t *)message.c_str(),
    message.length() + 1
  );

  return result == ESP_OK;
}

void SimpleEspNow::onReceive(ReceiveCallback callback) {
  receiveCallback = callback;
}

void SimpleEspNow::onSend(SendCallback callback) {
  sendCallback = callback;
}

void SimpleEspNow::receiveHandler(const uint8_t *mac, const uint8_t *data, int len) {
  String message = "";

  for (int i = 0; i < len; i++) {
    if (data[i] == '\0') break;
    message += (char)data[i];
  }

  if (receiveCallback != nullptr) {
    receiveCallback(message);
  }
}

void SimpleEspNow::sendHandler(const uint8_t *mac, esp_now_send_status_t status) {
  if (sendCallback != nullptr) {
    sendCallback(status == ESP_NOW_SEND_SUCCESS);
  }
}