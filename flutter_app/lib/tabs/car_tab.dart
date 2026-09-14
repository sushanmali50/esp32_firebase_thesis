import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'dart:math' as math;

class CarTab extends StatelessWidget {
  final String command;
  final int speedValue;
  final bool ledState;
  final void Function(String) sendCommand;

  final double pitch;
  final double roll;
  final int ultrasonic;
  final String irLeft;
  final String irRight;
  final double robotSpeed;
  final VoidCallback testAppToEspLatency;
  final VoidCallback testEspToAppLatency;

  final String mode;
  final void Function(String) setMode;

  final String taskStatus; // ✅ New

  const CarTab({
    required this.command,
    required this.speedValue,
    required this.ledState,
    required this.sendCommand,
    required this.pitch,
    required this.roll,
    required this.ultrasonic,
    required this.irLeft,
    required this.irRight,
    required this.robotSpeed,
    required this.mode,
    required this.setMode,
    required this.taskStatus,
    required this.testAppToEspLatency,
    required this.testEspToAppLatency,// ✅ New
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    final isClose = ultrasonic < 15;
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Card(
            elevation: 4,
            child: ListTile(
              title: const Text("🔌 Current Command"),
              subtitle: Text(command, style: const TextStyle(fontSize: 22)),
            ),
          ),
          const SizedBox(height: 20),
          const Text("🚦 Mode", style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
          Row(
            children: [
              const Text("Manual"),
              Switch(
                value: mode == "auto",
                onChanged: (val) => setMode(val ? "auto" : "manual"),
              ),
              const Text("Auto"),
            ],
          ),
          const SizedBox(height: 20),

          // ✅ Task Status Card
          Card(
            elevation: 4,
            margin: const EdgeInsets.symmetric(vertical: 12),
            child: ListTile(
              leading: const Icon(Icons.track_changes),
              title: const Text("Task Status"),
              subtitle: Text(taskStatus),
              tileColor: Colors.blue[50],
            ),
          ),

          const Text("📟 Sensor Data", style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
          const SizedBox(height: 10),
          Wrap(
            spacing: 12,
            runSpacing: 12,
            children: [
              _buildSensorCard("📐 Pitch", "${pitch.toStringAsFixed(1)}°"),
              _buildSensorCard("📐 Roll", "${roll.toStringAsFixed(1)}°"),
              _buildSensorCard("📏 Distance", "$ultrasonic cm", isClose ? Colors.red[100] : null),
              _buildSensorCard("👁️ IR Left", irLeft, irLeft == "Black" ? Colors.red[100] : null),
              _buildSensorCard("👁️ IR Right", irRight, irRight == "Black" ? Colors.red[100] : null),
              Card(
                elevation: 4,
                child: Padding(
                  padding: const EdgeInsets.all(8.0),
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      const Text("🧭 Speedometer", style: TextStyle(fontWeight: FontWeight.bold)),
                      const SizedBox(height: 8),
                      CustomPaint(size: const Size(100, 100), painter: SpeedometerPainter(speed: robotSpeed)),
                      const SizedBox(height: 4),
                      Text("${robotSpeed.toStringAsFixed(2)} m/s²", style: const TextStyle(fontSize: 14))
                    ],
                  ),
                ),
              ),
            ],
          ),
          const SizedBox(height: 20),
          const Text("🕹️ Control Robot", style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
          Column(
            children: [
              Center(child: ElevatedButton.icon(onPressed: () => sendCommand("forward"), icon: const Icon(Icons.arrow_upward), label: const Text("Forward"))),
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  ElevatedButton.icon(onPressed: () => sendCommand("left"), icon: const Icon(Icons.arrow_back), label: const Text("Left")),
                  const SizedBox(width: 8),
                  ElevatedButton.icon(onPressed: () => sendCommand("stop"), icon: const Icon(Icons.stop), label: const Text("Stop")),
                  const SizedBox(width: 8),
                  ElevatedButton.icon(onPressed: () => sendCommand("right"), icon: const Icon(Icons.arrow_forward), label: const Text("Right")),
                ],
              ),
              Center(child: ElevatedButton.icon(onPressed: () => sendCommand("backward"), icon: const Icon(Icons.arrow_downward), label: const Text("Backward"))),
            ],
          ),
          const SizedBox(height: 20),
          const Text("⚙️ Speed Control", style: TextStyle(fontSize: 20)),
          Slider(
            value: speedValue.toDouble(),
            min: 0,
            max: 255,
            divisions: 255,
            label: speedValue.toString(),
            onChanged: (value) {
              FirebaseDatabase.instance.ref("robot/speed").set(value.toInt());
            },
          ),
          const SizedBox(height: 20),
          ElevatedButton.icon(
            onPressed: testAppToEspLatency,
            icon: Icon(Icons.timer),
            label: Text("Test App → ESP Latency"),
          ),
          ElevatedButton.icon(
            onPressed: testEspToAppLatency,
            icon: Icon(Icons.timer_outlined),
            label: Text("Test ESP → App Latency"),
          ),

        ],
      ),
    );
  }

  Widget _buildSensorCard(String title, String value, [Color? color]) {
    return SizedBox(
      width: 160,
      child: Card(
        color: color,
        elevation: 4,
        child: Padding(
          padding: const EdgeInsets.all(12.0),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(title, style: const TextStyle(fontWeight: FontWeight.bold)),
              const SizedBox(height: 6),
              Text(value, style: const TextStyle(fontSize: 16)),
            ],
          ),
        ),
      ),
    );
  }
}

class SpeedometerPainter extends CustomPainter {
  final double speed;
  SpeedometerPainter({required this.speed});

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width / 2 - 10;
    final angle = (speed.clamp(0, 10) / 10) * math.pi;

    final backgroundPaint = Paint()
      ..color = Colors.grey[300]!
      ..strokeWidth = 10
      ..style = PaintingStyle.stroke;
    final needlePaint = Paint()
      ..color = Colors.blueAccent
      ..strokeWidth = 4;

    canvas.drawArc(Rect.fromCircle(center: center, radius: radius), math.pi, math.pi, false, backgroundPaint);

    final needleX = center.dx + radius * math.cos(math.pi + angle);
    final needleY = center.dy + radius * math.sin(math.pi + angle);
    canvas.drawLine(center, Offset(needleX, needleY), needlePaint);
  }

  @override
  bool shouldRepaint(covariant SpeedometerPainter oldDelegate) => oldDelegate.speed != speed;
}