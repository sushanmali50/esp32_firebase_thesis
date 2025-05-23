#ifndef SENSOR_READINGS_H
#define SENSOR_READINGS_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === Pin Definitions ===
// Define GPIO pins connected to sensors
#define IR_LEFT_PIN 35     // Left infrared sensor
#define IR_RIGHT_PIN 34    // Right infrared sensor
#define TRIG_PIN 18        // Ultrasonic trigger pin
#define ECHO_PIN 19        // Ultrasonic echo pin

// === setupSensors ===
// Initializes all sensor-related pins (IR and Ultrasonic)
void setupSensors() {
  pinMode(IR_LEFT_PIN, INPUT);
  pinMode(IR_RIGHT_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

// === getUltrasonicDistance ===
// Measures distance using an ultrasonic sensor (HC-SR04 compatible)
// Returns:
//   - Distance in centimeters based on time of flight calculation
int getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);  // Time taken for echo to return
  return duration * 0.034 / 2;              // Convert to distance in cm
}

// === setupMPU ===
// Initializes the MPU6050 sensor with custom ranges and bandwidth
// Parameters:
//   - mpu: Reference to the Adafruit_MPU6050 object
void setupMPU(Adafruit_MPU6050 &mpu) {
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);  // Halt execution if sensor is not found
  }

  // Configure sensor sensitivity
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

#endif  // SENSOR_READINGS_H
