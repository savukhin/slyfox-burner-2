import 'package:flutter/material.dart';

import 'layout/default_layout.dart';
import 'views/choose_com_view.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: const Color.fromARGB(255, 255, 190, 92)),
        useMaterial3: true,
      ),

      home: const DefaultLayout(
        child: ChooseComView(),
      ),
    );
  }
}
