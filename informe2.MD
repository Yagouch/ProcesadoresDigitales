# Informe 2. Práctica 1 #
---
### Código: Temperatura ###

```
#include <Arduino.h>

float measurement = 0;

void setup() {

  Serial.begin(115200);
  measurement = temperatureRead();

}

void loop() {
  
  Serial.print("Hall sensor measurement: ");
  Serial.println(measurement);

  delay(1000);

}
```

### Diagrama de flujo ###

<div class="mermaid">
flowchart TD
    A(Inicio) --> B[Asignar la función de lectura de temperatura]
    B --> C[1. Imprimir la lectura de temperatura]
    C --> D[Delay de 1s '1000ms']
    D --> C
</div>
Hecho con mermaid.

### Diagrama de tiempos ###
**Nota:** ipp=Imprimir por pantalla.
```wavedrom
{ signal : [
  {name: "accion", wave: "57575757", data: "1 delay 1 delay 1 delay 1 delay"},
  {name: "ipp", wave: "10101010"},
  {name: "lectura", wave: "1......."},
]}
```