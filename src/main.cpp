#include <Arduino.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
// Las tres línea anteriores comprueban que el Bluetooth esté correctamente habilitado.

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Inicializamos Bluetooth con un nombre de dispositivo
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() { //Aqui mandamos y recibimos información
  if (Serial.available()) { //Si hay bytes recibidos al móvil
    SerialBT.write(Serial.read()); //Escribimos esos datos via bluetooth al móvil
  }
  if (SerialBT.available()) { //Si hay bytes recibidos en el ESP32
    Serial.write(SerialBT.read()); //Los enviamos para ser impresos por pantalla
  }
  delay(20);
}