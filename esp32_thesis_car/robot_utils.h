#ifndef ROBOT_UTILS_H
#define ROBOT_UTILS_H

#include <Arduino.h>
#include <Firebase_ESP_Client.h>

// === Constants for IR Sensor Interpretation ===
#define BLACK "Black"
#define WHITE "White"

// === getIRState ===
// Reads an IR sensor and returns a string "Black" or "White"
// Useful for Firebase logging or display
// Parameters:
//   - pin: the GPIO pin connected to the IR sensor
//   - invert: set to true if the sensor output logic is inverted
inline String getIRState(int pin, bool invert = false) {
  int reading = digitalRead(pin);
  if (invert) reading = !reading;
  return reading == LOW ? BLACK : WHITE;
}

// === getIRBoolState ===
// Reads an IR sensor and returns a boolean indicating black detection
// Used in control logic for line following
// Parameters:
//   - pin: the GPIO pin connected to the IR sensor
//   - invert: set to true if the sensor output logic is inverted
// Returns:
//   - true if the sensor detects a black line, false otherwise
inline bool getIRBoolState(int pin, bool invert = false) {
  int reading = digitalRead(pin);
  return invert ? reading == HIGH : reading == LOW;  // true = sees black line
}

// === getStringFromFirebase ===
// Retrieves a string value from the Firebase Realtime Database
// Parameters:
//   - fbdo: Firebase data object reference
//   - path: Database path to fetch the string from
// Returns:
//   - The string value if successful, otherwise an empty string
inline String getStringFromFirebase(FirebaseData &fbdo, const String &path) {
  if (Firebase.RTDB.getString(&fbdo, path)) {
    return fbdo.stringData();
  }
  return "";
}

// === getIntFromFirebase ===
// Retrieves an integer value from the Firebase Realtime Database
// Parameters:
//   - fbdo: Firebase data object reference
//   - path: Database path to fetch the integer from
// Returns:
//   - The integer value if successful, otherwise 0
inline int getIntFromFirebase(FirebaseData &fbdo, const String &path) {
  if (Firebase.RTDB.getInt(&fbdo, path)) {
    return fbdo.intData();
  }
  return 0;
}

// === setInitialValues ===
// Initializes default robot state in Firebase on boot
// Parameters:
//   - fbdo: Firebase data object reference
//   - speed: The initial robot movement speed
inline void setInitialValues(FirebaseData &fbdo, int speed) {
  Firebase.RTDB.setString(&fbdo, "/robot/mode", "manual");
  Firebase.RTDB.setString(&fbdo, "/robot/status", "moving");
  Firebase.RTDB.setString(&fbdo, "/robot/command", "stop");
  Firebase.RTDB.setInt(&fbdo, "/robot/speed", speed);
}

#endif  // ROBOT_UTILS_H
