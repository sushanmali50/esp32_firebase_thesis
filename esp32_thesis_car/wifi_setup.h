#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>

// === Wi-Fi Credentials ===
#define WIFI_SSID "Legion_5"
#define WIFI_PASSWORD "Lenovolegion50"

// === connectWiFi ===
// Connects the ESP32 to a specified Wi-Fi network using the defined credentials.
// Displays connection progress via serial monitor.
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Initiate Wi-Fi connection

  // Wait until the connection is established
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");  // Print progress dots while connecting
  }

  // Print confirmation once connected
  Serial.println("\nWiFi connected");
}

#endif  // WIFI_SETUP_H
