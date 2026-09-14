import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'tabs/car_tab.dart';
import 'tabs/arm_tab.dart';
import 'tabs/logs_tab.dart';


class RobotDashboard extends StatefulWidget {
  const RobotDashboard({super.key});

  @override
  _RobotDashboardState createState() => _RobotDashboardState();
}

class _RobotDashboardState extends State<RobotDashboard> {
  final _commandRef = FirebaseDatabase.instance.ref("robot/command");
  final _speedRef = FirebaseDatabase.instance.ref("robot/speed");
  final _ledRef = FirebaseDatabase.instance.ref("control/led");
  final _servoRef = FirebaseDatabase.instance.ref("arm/servos");
  final _statusRef = FirebaseDatabase.instance.ref("arm/status");
  final _modeRef = FirebaseDatabase.instance.ref("robot/mode");

  String _command = "None";
  int _speedValue = 180;
  bool _ledState = false;
  String _mode = "manual";
  String _taskStatus = "Unknown";
  List<double> _servoAngles = List.filled(6, 90);

  double _pitch = 0.0, _roll = 0.0, _robotSpeed = 0.0;
  int _ultrasonic = 0;
  String _irLeft = "Unknown", _irRight = "Unknown";
  double? _lastPitch;
  DateTime? _lastUpdate;

  @override
  void initState() {
    super.initState();
    _listenForLatency();
    _commandRef.onValue.listen((e) => setState(() => _command = e.snapshot.value.toString()));
    _speedRef.onValue.listen((e) => setState(() => _speedValue = int.tryParse(e.snapshot.value.toString()) ?? 180));
    _ledRef.onValue.listen((e) => setState(() => _ledState = e.snapshot.value == true));
    _modeRef.onValue.listen((e) => setState(() => _mode = e.snapshot.value.toString()));

    FirebaseDatabase.instance.ref("robot/orientation/pitch").onValue.listen((event) {
      final currentPitch = double.tryParse(event.snapshot.value.toString()) ?? 0.0;
      final now = DateTime.now();
      if (_lastPitch != null && _lastUpdate != null) {
        final dt = now.difference(_lastUpdate!).inMilliseconds / 1000.0;
        final dp = (currentPitch - _lastPitch!).abs();
        setState(() => _robotSpeed = dp / dt);
      }
      _lastPitch = currentPitch;
      _lastUpdate = now;
      setState(() => _pitch = currentPitch);
    });

    FirebaseDatabase.instance.ref("robot/orientation/roll").onValue.listen((e) {
      setState(() => _roll = double.tryParse(e.snapshot.value.toString()) ?? 0.0);
    });
    FirebaseDatabase.instance.ref("robot/sensors/ultrasonic").onValue.listen((e) {
      setState(() => _ultrasonic = int.tryParse(e.snapshot.value.toString()) ?? 0);
    });
    FirebaseDatabase.instance.ref("robot/sensors/ir_left").onValue.listen((e) {
      setState(() => _irLeft = e.snapshot.value?.toString() ?? "Unknown");
    });
    FirebaseDatabase.instance.ref("robot/sensors/ir_right").onValue.listen((e) {
      setState(() => _irRight = e.snapshot.value?.toString() ?? "Unknown");
    });
    FirebaseDatabase.instance.ref("robot/status").onValue.listen((e) {
      setState(() => _taskStatus = e.snapshot.value?.toString() ?? "Unknown");
    });
  }

  void _listenForLatency() {
    final ref = FirebaseDatabase.instance.ref("/latency_test/esp_to_app");

    int? lastProcessedTimestamp;

    ref.onValue.listen((event) {
      final data = event.snapshot.value;
      if (data is! Map) return;

      final int? timestampSent = data["timestamp_sent"] is int
          ? data["timestamp_sent"] as int
          : int.tryParse(data["timestamp_sent"].toString());

      if (timestampSent == null) return;

      // Check if this timestamp has already been processed
      if (lastProcessedTimestamp == timestampSent) {
        return; // Avoid duplicate calculation
      }

      lastProcessedTimestamp = timestampSent;

      final int receivedTime = DateTime.now().toUtc().millisecondsSinceEpoch;
      final int latency = receivedTime - timestampSent;

      // Skip unrealistic latencies
      if (latency < 0 || latency > 5000) {
        print("⚠️ Unrealistic latency ignored: $latency ms");
        return;
      }

      print("📡 ESP → Firebase → App latency: $latency ms");

      FirebaseDatabase.instance
          .ref("/latency_test/esp_to_app/latency_logs")
          .push()
          .set({
        "sent": timestampSent,
        "received": receivedTime,
        "latency": latency,
      });
    });
  }


  void _sendAppToEspTimestamp() {
    final int now = DateTime.now().toUtc().millisecondsSinceEpoch;

    FirebaseDatabase.instance
        .ref("/latency_test/app_to_esp")
        .update({
      "timestamp_sent": now,
      "processed": false,
    }).then((_) => print("📤 Sent timestamp (ms) with processed=false"));
  }

  void _sendEspToAppTestRequest() {
    FirebaseDatabase.instance
        .ref("/latency_test/esp_to_app")
        .update({
      "request": true,
    }).then((_) => print("📤 Sent request for ESP → App latency test"));
  }


  void sendCommand(String cmd) => _commandRef.set(cmd);
  void toggleLED() {
    setState(() => _ledState = !_ledState);
    _ledRef.set(_ledState);
  }

  void setMode(String newMode) {
    setState(() => _mode = newMode);
    _modeRef.set(newMode);
  }

  void updateServo(int index, double angle) {
    setState(() => _servoAngles[index] = angle);
    _servoRef.update({"$index": angle.toInt()});
  }

  void triggerGripper(String action) => _statusRef.set(action);

  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      length: 3,
      child: Scaffold(
        appBar: AppBar(
          title: const Text('ESP32 Robot Dashboard'),
          actions: [
            IconButton(
              icon: const Icon(Icons.logout),
              tooltip: 'Logout',
              onPressed: () async {
                await FirebaseAuth.instance.signOut();
              },
            ),
          ],
          bottom: const TabBar(
            tabs: [
              Tab(text: "Car", icon: Icon(Icons.directions_car)),
              Tab(text: "Arm", icon: Icon(Icons.precision_manufacturing)),
              Tab(text: "Logs", icon: Icon(Icons.list_alt)),
            ],
          ),
        ),
        body: TabBarView(
          children: [
            CarTab(
              command: _command,
              speedValue: _speedValue,
              ledState: _ledState,
              sendCommand: sendCommand,
              pitch: _pitch,
              roll: _roll,
              ultrasonic: _ultrasonic,
              irLeft: _irLeft,
              irRight: _irRight,
              robotSpeed: _robotSpeed,
              mode: _mode,
              setMode: setMode,
              taskStatus: _mode == "manual" ? "Manual mode – No active task" : _taskStatus,
              testAppToEspLatency: _sendAppToEspTimestamp,
              testEspToAppLatency: _sendEspToAppTestRequest,
            ),
            ArmTab(
              servoAngles: _servoAngles,
              updateServo: updateServo,
              triggerGripper: triggerGripper,
            ),
            LogsTab(),
          ],
        ),
      ),
    );
  }
}