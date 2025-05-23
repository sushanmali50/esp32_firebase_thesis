#ifndef FIREBASE_SETUP_H
#define FIREBASE_SETUP_H

#include <Firebase_ESP_Client.h>

// === Firebase Configuration Constants ===
#define API_KEY "AIzaSyB0t_iUPn7eoqh4JjIeI5834KOlyDPGZL8"
#define DATABASE_URL "https://esp32-thesis-9627d-default-rtdb.europe-west1.firebasedatabase.app/"

// === setupFirebase ===
// Initializes the Firebase connection and performs anonymous sign-up.
// Parameters:
//   - config: Firebase configuration structure
//   - auth: Firebase authentication structure
//   - fbdo: Firebase data object reference
//   - signupOK: Output boolean indicating whether sign-up succeeded
void setupFirebase(FirebaseConfig &config, FirebaseAuth &auth, FirebaseData &fbdo, bool &signupOK) {
  // Set API key and database URL
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Use anonymous sign-in (empty credentials)
  auth.user.email = "";
  auth.user.password = "";

  Serial.println("\u23f3 Attempting Firebase signUp...");
  signupOK = Firebase.signUp(&config, &auth, "", "");  // Perform anonymous sign-up

  if (!signupOK) {
    Serial.print("\u274c signUp failed: ");
    Serial.println(config.signer.signupError.message.c_str());
    return;
  }

  // Start Firebase with given config and auth
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);  // Enable auto-reconnection

  // Wait for Firebase to be ready, timeout after 10 seconds
  Serial.println("\u23f3 Waiting for Firebase ready...");
  unsigned long startTime = millis();
  while (!Firebase.ready()) {
    if (millis() - startTime > 10000) {
      Serial.println("\u274c Firebase not ready after 10s");
      signupOK = false;
      return;
    }
    delay(200);
  }

  Serial.println("\u2705 Firebase is ready and authenticated!");
  signupOK = true;
}

#endif  // FIREBASE_SETUP_H
