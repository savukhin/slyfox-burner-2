import 'dart:io';

import 'package:burner/components/parametrized-input.dart';
import 'package:flutter/material.dart';

class ManageCncView extends StatefulWidget {
  const ManageCncView({super.key, required this.address});

  final String address;

  @override
  State<ManageCncView> createState() => _ManageCncView();
}


class _ManageCncView extends State<ManageCncView> {
  int speed = 100;

  @override
  Widget build(BuildContext context) {
    final ButtonStyle directionButtonsStyle = ElevatedButton.styleFrom(
      textStyle: const TextStyle(fontSize: 20),
      backgroundColor: Theme.of(context).colorScheme.surfaceVariant,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)));

    return Column(
        children: [
          ParametrizedInput(initValue: 19, label: "Speed", dimension: "mm/s", onChange: (value) => {}),
          Flexible(
            flex: 1,
            child: GridView.count(
                childAspectRatio: 3 / 3,
                crossAxisSpacing: 10,
                mainAxisSpacing: 10,
                crossAxisCount: 3,
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
          const SizedBox(height: 10),
          ElevatedButton(
            style: ButtonStyle(
              backgroundColor: MaterialStatePropertyAll<Color>(
                  Theme.of(context).colorScheme.inversePrimary),
            ),
            onPressed: () {},
            child: const Text("Page"),
          ),
        ]);
  }
}
