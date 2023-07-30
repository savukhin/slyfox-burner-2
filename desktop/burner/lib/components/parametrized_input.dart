import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class ParametrizedInput extends StatefulWidget {
  const ParametrizedInput(
      {super.key,
      required this.initValue,
      required this.label,
      required this.dimension,
      required this.onChange});

  final int initValue;
  final String label;
  final String dimension;
  final void Function(int value) onChange;

  @override
  State<ParametrizedInput> createState() => _ParametrizedInput();
}

class _ParametrizedInput extends State<ParametrizedInput> {
  final txt = TextEditingController(text: "");
  late FocusNode myFocusNode;

  @override
  void initState() {
    super.initState();
    txt.text = widget.initValue.toString();
    myFocusNode = FocusNode();
  }

  @override
  void dispose() {
    myFocusNode.dispose();

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        SizedBox(
          width: 50,
          child: Text(
            widget.label,
          ),
        ),
        const SizedBox(width: 10),
        Flexible(
          child: TextFormField(
            keyboardType: TextInputType.number,
            controller: txt,
            focusNode: myFocusNode,
            decoration: InputDecoration(
              filled: true,
              border: const OutlineInputBorder(
                borderRadius: BorderRadius.all(Radius.circular(10)),
              ),
              hintText: 'Type...',
              fillColor: Theme.of(context).colorScheme.inverseSurface,
            ),
            style: TextStyle(
              color: Theme.of(context).cardColor,
            ),
            inputFormatters: [
              FilteringTextInputFormatter.allow(RegExp(r'[1-9][0-9]*')),
            ],
            onChanged: (value) {
              var newValue = value;
              if (newValue == "") {
                newValue = "0";
              }
              if (newValue != value) {
                txt.text = newValue;
                // myFocusNode.requestFocus();
              }

              var myInt = int.parse(newValue);
              widget.onChange(myInt);
            },
          ),
        ),
        const SizedBox(width: 10),
        Text(widget.dimension),
      ],
    );
  }
}
