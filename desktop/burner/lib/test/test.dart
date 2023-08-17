// Import the test package and Counter class
import 'dart:ffi';
import 'dart:typed_data';
import 'package:test/test.dart';

class MyObj {
  int A = 1;
  int B = 2;
}

void main() {
  // Uint8 a = 2 as Uint8;

  // test('Counter value should be incremented', () {
  //   expect(a, 2);
  // });

  // var bytes = Uint8List.fromList([50, 0]);
  // var powerValue = bytes.buffer.asByteData().getUint16(0, Endian.little);
  // print(powerValue);

  // bytes = Uint8List.fromList([500000, 0]);
  // // powerValue = bytes.buffer.
  // ByteData.view(bytes.buffer);
  // // powerValue = bytes.buffer.asByteData().getUint16(0, Endian.little);
  // print(powerValue);

  DynamicLibrary.MyObj obj = MyObj();
  print(obj.toString());
}
