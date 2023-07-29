import 'package:flutter/material.dart';
import 'package:flutter_libserialport/flutter_libserialport.dart';

class ChooseComView extends StatefulWidget {
  const ChooseComView({super.key});

  @override
  State<ChooseComView> createState() => _ChooseComView();
}

class _ChooseComView extends State<ChooseComView> {
  String? dropdownValue;
  List<String> list = <String>['One', 'Two', 'Three', 'Four'];
  bool comChoosed = false;
  List<String> availablePorts = [];

  @override
  void initState() {
    super.initState();
    refreshComs();
  }
  

  void refreshComs() {
    setState(() => availablePorts = SerialPort.availablePorts);
  }

  bool connectCom() {
    return true;
  }

  void onConnectComPressed() {

  }

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.max,
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
      ElevatedButton(
        style: ButtonStyle(
          backgroundColor: MaterialStatePropertyAll<Color>(
              Theme.of(context).colorScheme.inversePrimary),
        ),
        onPressed: refreshComs,
        child: const Text("Refresh COMs"),
      ),
      const SizedBox(height: 10),

      DropdownButton<String>(
        value: dropdownValue,
        elevation: 1,
        hint: const Text("Choose an item"),
        isExpanded: true,
        underline: Container(
          height: 2,
          color: Theme.of(context).colorScheme.inversePrimary,
        ),
        onChanged: (String? value) {
          setState(() {
            dropdownValue = value ?? "";
            comChoosed = (value != null);
          });
        },
        items: availablePorts.map<DropdownMenuItem<String>>((String address) {
          // final port = SerialPort(address);

          return DropdownMenuItem<String>(
            value: address,
            child: Center(child: Text(address)),
          );
        }).toList(),
      ),
      const SizedBox(height: 10),

      comChoosed ? ElevatedButton(
        style: ButtonStyle(
          backgroundColor: MaterialStatePropertyAll<Color>(
              Theme.of(context).colorScheme.inversePrimary),
        ),
        onPressed: onConnectComPressed,
        child: const Text("Connect COM"),
      ) : const Text(""),
      
    ]);
  }
}
