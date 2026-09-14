import 'package:flutter/material.dart';

class ArmTab extends StatelessWidget {
  final List<double> servoAngles;
  final void Function(int, double) updateServo;
  final void Function(String) triggerGripper;

  const ArmTab({
    required this.servoAngles,
    required this.updateServo,
    required this.triggerGripper,
    super.key,
  });

  @override
  Widget build(BuildContext context) {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text("🤖 Robot Arm Control", style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
          Column(
            children: List.generate(6, (index) {
              final angle = servoAngles[index];
              return Column(
                children: [
                  Text("Servo $index: ${angle.toInt()}°"),
                  Slider(
                    value: angle,
                    min: 0,
                    max: 180,
                    divisions: 180,
                    label: angle.toStringAsFixed(0),
                    onChanged: (value) => updateServo(index, value),
                  ),
                ],
              );
            }),
          ),
          const SizedBox(height: 20),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              ElevatedButton(onPressed: () => triggerGripper("grab"), child: const Text("Grab")),
              const SizedBox(width: 20),
              ElevatedButton(onPressed: () => triggerGripper("release"), child: const Text("Release")),
            ],
          ),
        ],
      ),
    );
  }
}