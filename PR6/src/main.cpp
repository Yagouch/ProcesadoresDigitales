#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

#define SS_RFID 10
#define RST_PIN 9
#define SS_SD 5

MFRC522 rfid(SS_RFID, RST_PIN); // Creamos un objeto MFRC522
File myFile;

void setup() {
  Serial.begin(115200);

  SPI.begin(); // Inicializamos el bus SPI
  rfid.PCD_Init(); // Inicializamos el lector RFID

  Serial.println("Lector RFID iniciado");

  if (!SD.begin(SS_SD)) { // Inicializamos la tarjeta SD
    Serial.println("Error al iniciar la tarjeta SD");
    return;
  }

  Serial.println("Tarjeta SD iniciada");
}

void loop() {
  // Esperamos a que se detecte una etiqueta
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Obtenemos el UID de la etiqueta
  String content = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }

  // Escribimos el UID en la tarjeta SD
  myFile = SD.open("lecturas.log", FILE_WRITE);
  if (myFile) {
    myFile.print(content);
    myFile.println();
    myFile.close();

  }
  else {
    Serial.println("Error al abrir el archivo");
  }

  // Mostramos el UID por el monitor serial
  Serial.println("Lectura RFID:");
  Serial.println(content);
}
