import 'package:burner/views/manage_cnc_view.dart';
import 'package:flutter/material.dart';

import 'layout/default_layout.dart';
import 'views/choose_com_view.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyApp();
}

class _MyApp extends State<MyApp> {
  String? address;

  void onComConnectedCallback(String address) {
    setState(() {
      this.address = address;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(
            seedColor: const Color.fromARGB(255, 255, 190, 92)),
        useMaterial3: true,
      ),
      home: DefaultLayout(
        child: address == null ? ChooseComView(
          onComConnected: onComConnectedCallback,
        ) : ManageCncView(address: address!,),
      ),
    );
  }
}
