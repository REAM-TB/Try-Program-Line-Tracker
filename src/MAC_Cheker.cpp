#include <Arduino.h>
#include <WiFi.h>

#include "MAC_Cheker.h"

void MAC_Cheker() {
  WiFi.mode(WIFI_STA);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}