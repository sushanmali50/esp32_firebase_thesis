#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

// === Motor Driver Pin Definitions ===
// These are GPIO pins connected to the motor driver inputs
#define AIN2 27     // Motor A input 2
#define AIN1 26     // Motor A input 1
#define PWMA 14     // Motor A PWM speed control
#define BIN1 25     // Motor B input 1
#define BIN2 33     // Motor B input 2
#define PWMB 12     // Motor B PWM speed control
#define STBY 32     // Standby pin to enable/disable motor driver

// === setupMotors ===
// Configures all motor-related pins as outputs and enables motor driver
void setupMotors() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT); 
  digitalWrite(STBY, HIGH);  // Enable motor driver by pulling STBY high
}

// === move ===
// Controls motor direction and speed based on the given command.
// Parameters:
//   - cmd: Direction command ("forward", "backward", "left", "right", or other for stop)
//   - currentSpeed: PWM speed value (0-255)
void move(String cmd, int currentSpeed) {
  if (cmd == "forward") {
    // Move both motors forward
    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  } else if (cmd == "backward") {
    // Move both motors backward
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
  } else if (cmd == "left") {
    // Rotate left: left motor backward, right motor forward
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  } else if (cmd == "right") {
    // Rotate right: left motor forward, right motor backward
    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
  } else {
    // Stop motors
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
  }

  // Apply speed using PWM
  analogWrite(PWMA, currentSpeed);
  analogWrite(PWMB, currentSpeed);
}

#endif  // MOTOR_CONTROL_H
