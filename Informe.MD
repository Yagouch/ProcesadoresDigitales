```
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
  digitalWrite(23,HIGH); //Asignamos un valor alto de voltaje al pin 23 (LED)
  delay(1000); //Esperamos 1s
  Serial.println("OFF"); //Imprimimos por pantalla "OFF"
  digitalWrite(23,LOW); //Asignamos un valor bajo de voltaje (apagado) al pin 23 (LED)
  delay(2000); //Esperamos 2s
}

```