# Informe. Práctica 1 #
---
### Código ###
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

### Diagrama de flujo ###

<div class="mermaid">
flowchart TD
    A(Inicio) --> B[Declarar el pin 23 como salida]
    B --> C[1. Lectura nivel de voltaje  : Pin 15]
    C --> D[2. Imprimir por pantalla valor de lectura]
    D --> E[3. Imprimir por pantalla 'ON']
    E --> F[4. Asignar a pin 23 nivel alto de voltaje]
    F --> G[Delay de 1s '1000ms']
    G --> H[5. Imprimir por pantalla 'OFF']
    H --> I[6. Asignar a pin 23 nivel bajo de voltaje]
    I --> J[Delay de 2s '2000ms']
    J --> C
</div>

### Diagrama de tiempos ###
**Nota:** Como no sé cuantos ciclos de reloj hay en un segundo, intuimos que pasan más ciclos de los que visualmente se ven. (ipp=Imprimir por pantalla)
```wavedrom
{ signal : [
  { name: "actions",  wave: "45537..537....",   data: "1 2 3 4 delay:1s 5 6 delay:2s" },
  {name: "led", wave: "0...1....0...."},
  {name: "ipp", wave: "01.0...10....."},
  {name: "lectura", wave: "10............"},
]}
```