import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

class LogsTab extends StatefulWidget {
  const LogsTab({super.key});

  @override
  State<LogsTab> createState() => _LogsTabState();
}

class _LogsTabState extends State<LogsTab> {
  final _logsRef = FirebaseDatabase.instance.ref("robot/logs");
  List<String> logs = [];

  @override
  void initState() {
    super.initState();
    _logsRef.onValue.listen((event) {
      final data = event.snapshot.value as Map?;
      if (data == null) {
        setState(() => logs = ["🕵️ No logs yet"]);
        return;
      }

      final entries = data.entries.toList()
        ..sort((a, b) => b.key.compareTo(a.key)); // newest first

      setState(() => logs = entries.map((e) => "${e.key}: ${e.value}").toList());
    });
  }

  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      padding: const EdgeInsets.all(16),
      itemCount: logs.length,
      itemBuilder: (context, index) {
        return Card(
          elevation: 3,
          margin: const EdgeInsets.symmetric(vertical: 8),
          child: ListTile(
            leading: const Icon(Icons.event_note),
            title: Text(logs[index]),
          ),
        );
      },
    );
  }
}