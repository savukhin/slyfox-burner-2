import 'dart:io';

import 'package:burner/components/parametrized_input.dart';
import 'package:flutter/material.dart';

class ManageCncView extends StatefulWidget {
  const ManageCncView({super.key, required this.address});

  final String address;

  @override
  State<ManageCncView> createState() => _ManageCncView();
}

class _ManageCncView extends State<ManageCncView> {
  int speed = 100;
  static const double elemMargin = 20;

  @override
  Widget build(BuildContext context) {
    final ButtonStyle directionButtonsStyle = ElevatedButton.styleFrom(
        textStyle: const TextStyle(fontSize: 20),
        backgroundColor: Theme.of(context).colorScheme.surfaceVariant,
        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)));

    return Column(children: [
      ParametrizedInput(
          initValue: 19,
          label: "Step",
          dimension: "mm",
          onChange: (value) => {}),
      const SizedBox(height: elemMargin),
      SizedBox(
        // flex: 1,
        height: 170,
        child: GridView.count(
            primary: false,
            // childAspectRatio: 3 / 3,
            crossAxisSpacing: 10,
            mainAxisSpacing: 10,
            crossAxisCount: 3,
            // padding: EdgeInsets.all(10),
            shrinkWrap: true,
            children: [
              Container(),
              ElevatedButton(
                  onPressed: () {},
                  style: directionButtonsStyle,
                  child: const Text("↑")),
              Container(),
              ElevatedButton(
                  onPressed: () {},
                  style: directionButtonsStyle,
                  child: const Text("←")),
              ElevatedButton(
                  onPressed: () {},
                  style: directionButtonsStyle,
                  child: const Text("↓")),
              ElevatedButton(
                  onPressed: () {},
                  style: directionButtonsStyle,
                  child: const Text("→")),
            ]),
      ),
      // const SizedBox(height: elemMargin),
      Divider(
        color: Theme.of(context).dividerColor,
      ),
      ParametrizedInput(
          initValue: 100,
          label: "Rapid X Speed",
          dimension: "mm/s",
          onChange: (value) => {}),
      const SizedBox(height: elemMargin),
      ParametrizedInput(
          initValue: 19,
          label: "Rapid Y Speed",
          dimension: "mm/s",
          onChange: (value) => {}),
      const SizedBox(height: elemMargin),
      ParametrizedInput(
          initValue: 19,
          label: "Low X Speed",
          dimension: "mm/s",
          onChange: (value) => {}),
      const SizedBox(height: elemMargin),
      ParametrizedInput(
          initValue: 19,
          label: "Low Y Speed",
          dimension: "mm/s",
          onChange: (value) => {}),
      Divider(
        color: Theme.of(context).dividerColor,
      ),
      ParametrizedInput(
          initValue: 10,
          label: "Accel X",
          dimension: "mm/s^2",
          onChange: (value) => {}),
      const SizedBox(height: elemMargin),
      ParametrizedInput(
          initValue: 10,
          label: "Accel Y",
          dimension: "mm/s^2",
          onChange: (value) => {}),
      Divider(
        color: Theme.of(context).dividerColor,
      ),
      ElevatedButton(
        style: ButtonStyle(
          backgroundColor: MaterialStatePropertyAll<Color>(
              Theme.of(context).colorScheme.inversePrimary),
        ),
        onPressed: () {},
        child: const Text("StartExperiment"),
      ),
    ]);
  }
}
