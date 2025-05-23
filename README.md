# 🤖 Real-Time Multi-Device Robot Coordination via Firebase (Bachelor Thesis Project)

This project demonstrates real-time coordination between an ESP32-controlled mobile robot and a Raspberry Pi-powered robotic arm using [Google Firebase Realtime Database](https://firebase.google.com/products/realtime-database) as a cloud-based broker. Developed as part of my Bachelor Thesis at Constructor University Bremen, the system showcases low-cost Internet of Things (IoT) robotics with cloud-triggered multi-agent communication.

## 📌 Project Overview

- **Mobile Robot:** Line-following robot car based on ESP32 microcontroller.
- **Robotic Arm:** 6DOF servo-controlled arm driven by Raspberry Pi.
- **Firebase:** Central cloud hub for task coordination, status updates, and sensor streaming.
- **Flutter App:** Mobile interface to monitor sensor data, manually control the robot, view logs, and toggle auto/manual modes.

This architecture is designed around **event-driven coordination**, allowing independent agents to communicate indirectly through Firebase.

## ✨ Features

- 🔄 Real-time cloud coordination between mobile robot and robotic arm
- 📡 Firebase integration for low-latency data exchange
- 📱 Mobile app interface with live sensor feed and task control (built in Flutter)
- 🛑 Checkpoint detection via dual IR sensors
- 🧠 Pitch/Roll reporting using MPU6050 gyroscope
- 📈 Logging of latency metrics, memory usage, and task durations
- 📂 Lightweight modular code structure for embedded development

## 🔧 Tech Stack

| Component         | Description                                     |
|------------------|-------------------------------------------------|
| **ESP32**        | Main controller for mobile robot                |
| **Raspberry Pi** | Host for robotic arm control and PWM sequencing |
| **Firebase RTDB**| Cloud data broker for device coordination       |
| **Flutter**      | Mobile app frontend                             |
| **MPU6050**      | Sensor for pitch and roll                       |
| **Ultrasonic**   | Distance measurement for obstacle avoidance     |
| **IR Sensors**   | Line-following and checkpoint detection         |

## 📷 System Architecture

```
          +--------------------+
          |  Mobile App        |
          |  (Flutter)         |
          +--------+-----------+
                   |
         commands & updates
                   v
         +---------+----------+
         |    Firebase RTDB   |
         +---------+----------+
           ^       |       ^
           |       |       |
 sensor    |  status/   task trigger
  logs     |  commands       |
           |       v       |
       +---+---+       +---+---+
       | ESP32 |       |  RPi   |
       | Robot |       |  Arm   |
       +-------+       +-------+
           |               |
     sensor data     task_done flag
           +---------------+
```





## 📁 Folder Structure

```bash
firebase-robot-coordination/
├── src/
│   ├── esp32_thesis_car.ino          # ESP32 firmware
│   ├── firebase_setup.h             # Auth & DB config
│   ├── motor_control.h              # PWM motor control
│   ├── sensor_readings.h            # Sensor helpers
│   └── robot_utils.h                # Logging, status tools
├── app/
│   └── robot_flutter_app/           # Flutter UI code
├── arm/
│   └── robot_arm_thesis.py    # Raspberry Pi servo control
├── assets/
│   ├── demo_video/                 # Video of robot setup
│
└── README.md
```

---

## 🚀 Getting Started

### 1. Hardware Setup

- 1x ESP32 Dev Board  
- 2x IR Sensors (Line following)  
- 1x Ultrasonic Sensor  
- 1x MPU6050 (GY-521)  
- 2x DC Motors + TB6612FNG Motor Driver  
- 1x Raspberry Pi (any model with I2C)  
- 6x Servo Motors + PCA9685 driver  

### 2. Firebase Configuration

- Create a new Firebase project  
- Enable Realtime Database and Authentication  
- Use **Anonymous Auth** for ESP32 and **Email/Password** for Flutter app  
- Copy API keys and database URLs into `firebase_setup.h`  

### 3. Flash ESP32 Firmware

```bash
# Using Arduino IDE or PlatformIO
Open `esp32_thesis_car.ino` and flash to ESP32
```

### 4. Run Robotic Arm (Raspberry Pi)

```bash
python3 robot_arm_thesis.py
```

### 5. Launch Mobile App

```bash
cd app/robot_flutter_app
flutter run
```

---

## 📊 Key Metrics

| Metric                    | Value                            |
|---------------------------|----------------------------------|
| Ultrasonic Update Interval| ~590 ms (1.7 Hz avg)             |
| App-to-ESP Latency        | Avg: 326 ms, Min: 16 ms, Max: 889 ms |
| Memory Usage (ESP32)      | 203.2–203.6 KB free heap         |

---

## 📚 Academic Reference

This work is part of the Bachelor Thesis:  
**"Real-Time Data Visualization and Coordination in Low-Cost IoT Robots via Firebase"**  
Constructor University Bremen — School of Computer Science and Engineering (2025)  
**Supervisor:** Prof. Dr. Fangning Hu

---

## 🧠 Future Work

- MQTT/Edge hybrid model for improved offline resilience  
- Closed-loop control for arm tasks  
- Integration with on-device camera + image recognition  
- Expanded multi-agent coordination (multiple robots/arms)  

---

## 📸 Demo Gallery

*(Add photos or GIFs here from `assets/demo_photos/`)*

---

## 📝 License

This project is developed for academic and educational purposes. Feel free to fork and adapt with credit.

---

## 👤 Author

**Sushan Mali**  
Bachelor of Science in Robotics  
Constructor University Bremen
