// === Include Libraries ===
#include <WiFi.h>                         // For Wi-Fi connectivity
#include <Wire.h>                         // For I2C communication
#include <Firebase_ESP_Client.h>         // Firebase client library for ESP32
#include "addons/RTDBHelper.h"           // Firebase Realtime DB helper functions
#include "addons/TokenHelper.h"          // Token handling for Firebase authentication
#include <Adafruit_MPU6050.h>            // MPU6050 sensor library for motion data
#include <Adafruit_Sensor.h>             // Unified sensor abstraction
#include <time.h>                         // For real-time clock and timestamps

// === Project-specific Modules ===
#include "robot_utils.h"                 // Utility functions for the robot
#include "wifi_setup.h"                  // Wi-Fi connection setup
#include "firebase_setup.h"              // Firebase initialization
#include "time_sync.h"                   // Time synchronization over NTP
#include "motor_control.h"               // Motor movement functions
#include "sensor_readings.h"             // Sensor data acquisition functions

// === Firebase Globals ===
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;                   // Track if Firebase sign-up succeeded

// === Sensor & State Variables ===
Adafruit_MPU6050 mpu;
int currentSpeed = 180;
String command = "", mode = "manual", status = "moving";
int distanceCM = 100;
bool waitingForTask = false;

static float lastPitch = 0.0, lastRoll = 0.0;
const float threshold = 1.0;             // Orientation update threshold

// === Timing Variables ===
unsigned long lastFirebaseUpdate = 0;
const unsigned long firebaseInterval = 300;     // Firebase update interval
unsigned long lastMemoryCheck = 0;
const unsigned long memoryCheckInterval = 5000; // Memory diagnostics interval
unsigned long waitingStartTime = 0;             // Track waiting start time

// === IR Sensor State Checks ===
bool isLeftBlack() { return digitalRead(IR_LEFT_PIN) == LOW; }
bool isRightBlack() { return digitalRead(IR_RIGHT_PIN) == LOW; }

// === Initial Setup Function ===
void setup() {
  Serial.begin(115200);
  setupMotors();
  setupSensors();
  setupMPU(mpu);
  connectWiFi();
  initTime();
  setupFirebase(config, auth, fbdo, signupOK);
  setInitialValues(fbdo, currentSpeed); // Set initial Firebase state

  Serial.printf("⏱️ Initial time (sec): %lu\n", time(nullptr));
  Serial.printf("⚙️ ESP32 CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
}

// === Firebase Logging Utility ===
void logEvent(const String &message) {
  time_t now = time(nullptr);
  struct tm *tm_info = localtime(&now);
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", tm_info);
  String path = "/robot/logs/" + String(buffer);

  if (Firebase.RTDB.setString(&fbdo, path, message)) {
    Serial.println("✅ Log: " + message);
  } else {
    Serial.println("❌ Log failed: " + fbdo.errorReason());
  }
}

// === Main Loop ===
void loop() {
  unsigned long now = millis();

  // --- Ultrasonic Sensor Update Monitoring ---
  static int lastUltrasonic = 0;
  static unsigned long lastUltrasonicUpdate = 0;
  int currentUltrasonic = getUltrasonicDistance();
  if (currentUltrasonic != lastUltrasonic) {
    unsigned long delta = now - lastUltrasonicUpdate;
    Firebase.RTDB.pushInt(&fbdo, "/diagnostics/ultrasonic_update_times", delta);
    Serial.printf("📡 Ultrasonic updated: Δ%lu ms\n", delta);
    lastUltrasonicUpdate = now;
    lastUltrasonic = currentUltrasonic;
  }
  distanceCM = currentUltrasonic;

  // --- Autonomous Line-Following and Checkpoint Logic ---
  if (mode == "auto" && !waitingForTask) {
    bool leftBlack = getIRBoolState(IR_LEFT_PIN);
    bool rightBlack = getIRBoolState(IR_RIGHT_PIN, true);

    if (distanceCM <= 20) {
      move("stop", currentSpeed); // Obstacle detected
    } else if (leftBlack && rightBlack) {
      move("stop", 0);  // Checkpoint reached
      Firebase.RTDB.setString(&fbdo, "/robot/status", "waiting");
      logEvent("📍 Checkpoint reached. Waiting for arm task.");
      waitingStartTime = millis();
      waitingForTask = true;
    } else if (leftBlack) {
      move("left", 100);
    } else if (rightBlack) {
      move("right", 100);
    } else {
      move("forward", currentSpeed);
    }
  }

  // --- Firebase Update Section ---
  if (now - lastFirebaseUpdate >= firebaseInterval && Firebase.ready() && signupOK) {
    lastFirebaseUpdate = now;

    // -- App → Firebase → ESP Latency Check --
    unsigned long sentTimeMs = 0;
    bool processed = true;
    if (Firebase.RTDB.getInt(&fbdo, "/latency_test/app_to_esp/timestamp_sent")) {
      sentTimeMs = fbdo.intData();
    }
    if (Firebase.RTDB.getBool(&fbdo, "/latency_test/app_to_esp/processed")) {
      processed = fbdo.boolData();
    }
    if (!processed && sentTimeMs > 0) {
      time_t currentSec = time(nullptr);
      if (currentSec > 1700000000) { // Valid time check
        unsigned long currentTimeMs = currentSec * 1000UL;
        long latencyMs = (long)currentTimeMs - (long)sentTimeMs;
        if (latencyMs >= 0 && latencyMs <= 5000) {
          Serial.printf("✅ App → Firebase → ESP latency (ms): %ld\n", latencyMs);
          Firebase.RTDB.pushInt(&fbdo, "/latency_test/app_to_esp/latency_logs", latencyMs);
          Firebase.RTDB.setBool(&fbdo, "/latency_test/app_to_esp/processed", true);
        }
      }
    }

    // -- ESP → Firebase → App Latency Check --
    bool espRequest = false;
    if (Firebase.RTDB.getBool(&fbdo, "/latency_test/esp_to_app/request")) {
      espRequest = fbdo.boolData();
    }
    if (espRequest) {
      time_t currentSec = time(nullptr);
      if (currentSec > 1700000000) {
        unsigned long nowTimeMs = currentSec * 1000UL;
        Firebase.RTDB.setInt(&fbdo, "/latency_test/esp_to_app/timestamp_sent", nowTimeMs);
        Firebase.RTDB.setBool(&fbdo, "/latency_test/esp_to_app/request", false);
        Serial.println("📤 ESP → App latency timestamp sent.");
      }
    }

    // -- Sensor and Status Updates --
    mode = getStringFromFirebase(fbdo, "/robot/mode");
    currentSpeed = getIntFromFirebase(fbdo, "/robot/speed");

    Firebase.RTDB.setString(&fbdo, "/robot/sensors/ir_left", isLeftBlack() ? "Black" : "White");
    Firebase.RTDB.setString(&fbdo, "/robot/sensors/ir_right", isRightBlack() ? "Black" : "White");
    Firebase.RTDB.setInt(&fbdo, "/robot/sensors/ultrasonic", distanceCM);

    // --- Orientation (Pitch & Roll) Update ---
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float pitch = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
    float roll = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

    if (abs(pitch - lastPitch) > threshold) {
      Firebase.RTDB.setFloat(&fbdo, "/robot/orientation/pitch", pitch);
      lastPitch = pitch;
    }
    if (abs(roll - lastRoll) > threshold) {
      Firebase.RTDB.setFloat(&fbdo, "/robot/orientation/roll", roll);
      lastRoll = roll;
    }

    // --- Manual Control via App ---
    if (mode == "manual") {
      command = getStringFromFirebase(fbdo, "/robot/command");
      move(command, currentSpeed);
    }

    // --- Task Completion Handling ---
    if (mode == "auto" && waitingForTask) {
      status = getStringFromFirebase(fbdo, "/robot/status");
      if (status == "task_done") {
        unsigned long completionTime = millis() - waitingStartTime;
        Serial.printf("⏱️ Task completion time: %lu ms\n", completionTime);
        Firebase.RTDB.pushInt(&fbdo, "/task_metrics/completion_times", completionTime);
        logEvent("✅ Task completed in " + String(completionTime) + " ms");
        move("forward", currentSpeed);
        delay(400);
        Firebase.RTDB.setString(&fbdo, "/robot/status", "moving");
        waitingForTask = false;
      }
    }
  }

  // --- Memory Usage Monitoring ---
  if (now - lastMemoryCheck >= memoryCheckInterval && Firebase.ready() && signupOK) {
    lastMemoryCheck = now;
    int freeHeap = ESP.getFreeHeap();
    Serial.printf("📉 Free Heap: %d bytes\n", freeHeap);
    Firebase.RTDB.pushInt(&fbdo, "/metrics/memory_usage", freeHeap);
  }
}
