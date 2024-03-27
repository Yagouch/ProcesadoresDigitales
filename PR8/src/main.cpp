#include <Arduino.h>
#include <HardwareSerial.h>


uint8_t byte_in = 0;    // Variable para almacenar el byte recibido

void setup() {

Serial.begin(115200);   // Inicialización de la comunicación serial
Serial2.begin(115200);  // Inicialización de la comunicación serial del UART2
Serial2.write(byte_in); // Envío de un byte por el UART2

}

void loop() {

if (Serial2.available() > 0) {  // Si hay datos disponibles en el UART2

    byte_in = Serial2.read();   // Almacenar el byte recibido en la variable byte_in
    Serial.println(byte_in);    // Imprimir el byte recibido en el UART0
    Serial2.write(byte_in+10);  // Enviar el byte recibido + 10 por el UART2


    delay(1000);               // Retardo de 1 segundo

}
}