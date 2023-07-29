import 'package:flutter/material.dart';

const List<String> list = <String>['One', 'Two', 'Three', 'Four'];

class ChooseComView extends StatefulWidget {
  const ChooseComView({super.key});

  @override
  State<ChooseComView> createState() => _ChooseComView();
}

class _ChooseComView extends State<ChooseComView> {
  String dropdownValue = list.first;

  @override
  Widget build(BuildContext context) {
    return Container(
      child: DropdownButton<String>(
        value: dropdownValue,
        icon: const Icon(Icons.access_alarm_sharp),
        elevation: 1,
        // style: const TextStyle(color: Colors.deepPurple),
        underline: Container(
          height: 2,
          // color: Colors.deepPurpleAccent,
        ),
        onChanged: (String? value) {
          // This is called when the user selects an item.
          setState(() {
            dropdownValue = value!;
          });
        },
        items: list.map<DropdownMenuItem<String>>((String value) {
          return DropdownMenuItem<String>(
            value: value,
            child: Text(value),
          );
        }).toList(),
      ),
    );
  }
}
