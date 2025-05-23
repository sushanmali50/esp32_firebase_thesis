#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <time.h>

// === initTime ===
// Initializes and synchronizes the ESP32 system time using NTP servers.
// This function ensures that a valid UNIX timestamp is retrieved before proceeding.
// Uses public NTP servers for global time accuracy.
//
// Behavior:
// - Tries to sync time with "pool.ntp.org" and "time.nist.gov"
// - Waits until the time is successfully retrieved or until a timeout (15 seconds)
// - Logs the current UNIX timestamp once synced
void initTime() {
  // Configure NTP servers (no timezone offset, use defaults)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  int attempts = 0;

  // Try fetching valid time, retry up to 15 times (15 seconds)
  while (!getLocalTime(&timeinfo) || time(nullptr) < 1700000000) {
    Serial.print("⏱️ Waiting for NTP time sync... ");
    Serial.println(time(nullptr));
    delay(1000);  // wait 1 second before retry
    if (++attempts > 15) {
      Serial.println("❌ NTP sync timeout after 15 seconds!");
      return;
    }
  }

  // Print the valid synced UNIX timestamp
  time_t now = time(nullptr);
  Serial.print("✅ NTP time synced: ");
  Serial.println(now);
}

#endif  // TIME_SYNC_H
