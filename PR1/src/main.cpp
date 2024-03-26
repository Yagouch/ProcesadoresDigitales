#include <Arduino.h>

const int potPin = 15; //Pin al que conectaremos el voltaje
int potValue = 0; //Variable donde se almacenará los valores de voltaje
float measurement = 0; //Variable donde se almacenará los valores de temperatura

void setup() {
  Serial.begin(115200);

  pinMode(23, OUTPUT); //Declarar el pin 23 como salida (LED)
}

void loop() {
  potValue=analogRead(potPin); //Lectura del valor analógico del punto 15 (Voltaje)
  Serial.println(potValue); //Escritura del valor obtenido antes
  
  Serial.println("ON"); //Imprimimos por pantalla "ON"
  digitalWrite(23,HIGH); //
  delay(1000);
  Serial.println("OFF");
  digitalWrite(23,LOW);
  delay(2000);
}

