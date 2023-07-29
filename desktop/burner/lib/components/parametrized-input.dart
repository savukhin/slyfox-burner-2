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
  final void Function(String value) onChange;

  @override
  State<ParametrizedInput> createState() => _ParametrizedInput();
}

class _ParametrizedInput extends State<ParametrizedInput> {
  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Text(widget.label),
        Flexible(
          child: TextFormField(
            keyboardType: TextInputType.number,
            decoration: const InputDecoration(
              border: OutlineInputBorder(),
              hintText: 'Type...',
            ),
            inputFormatters: [
              FilteringTextInputFormatter.digitsOnly
            ],
            onChanged: widget.onChange,
          ),
        ),
        Text(widget.dimension),
      ],
    );
  }
}
