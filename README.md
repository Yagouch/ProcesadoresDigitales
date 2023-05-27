# PRACTICA 7 - I2S  
## Yago Carballo Barroso y Ramon Llobet Duch
En esta práctica vamos a trabajar con el protocolo de comunicación I2S para la ESP32, el cual se usa para transferir señales de sonido digitales.
Así pues, reproduciremos música des de la memoria interna, desde una targeta SD, retransmitiremos una emisora de radio en directo y por último realizaremos un analizador de espectro con un micrófono I2S.

***
## Reproducción desde memoria interna (Archivo aac desde PROGMEM)
El siguiente código configura la ESP32 para generar sonido utilizando un archivo de audio en formato AAC. 
El archivo de audio se lee desde la memoria de programa y se envía a la salida de audio utilizando el protocolo I2S. 
El bucle principal del programa verifica si el generador de audio está en funcionamiento y, si es así, genera el audio.
Si el generador de audio se detiene, se muestra un mensaje y se espera un segundo antes de reiniciar el bucle.

```ino
//Bibliotecas
#include<Arduino.h>//biblioteca principal de Arduino para la placa ESP32
#include "AudioGeneratorAAC.h"//biblioteca para generar audio a partir de archivos AAC
#include "AudioOutputI2S.h"//biblioteca para la salida de audio utilizando el protocolo I2S
#include "AudioFileSourcePROGMEM.h"//biblioteca para leer archivos de audio almacenados en la memoria de programa (PROGMEM)
#include "sampleaac.h"

AudioFileSourcePROGMEM *in;//Crea un puntero a un objeto de tipo 
AudioGeneratorAAC *aac;//para generar audio a partir del archivo AAC.
AudioOutputI2S *out;//Crea un puntero a un objeto de tipo AudioOutputI2S para la salida de audio utilizando el protocolo I2S.

void setup(){
  Serial.begin(115200);//Inicializamos la comunicación serie con una velocidad de 115200 baudios para imprimir mensajes en el monitor serie.

  in = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));
  // Crea una instancia del objeto AudioFileSourcePROGMEM y asigna la dirección y el tamaño del archivo de audio almacenado en la memoria de programa.
  aac = new AudioGeneratorAAC();//Crea una instancia del objeto AudioGeneratorAAC para generar audio a partir del archivo AAC.
  out = new AudioOutputI2S();//Crea una instancia del objeto AudioOutputI2S para la salida de audio utilizando el protocolo I2S.
  out -> SetGain(0.25);//Establece el nivel de ganancia de salida de audio en 0.25 (25%).
  out -> SetPinout(26,25,22);//Configuramos los pines GPIO 26, 25 y 22 para la salida de audio utilizando el protocolo I2S
  aac->begin(in, out);//Inicializa el generador de audio AAC con la fuente de audio y la salida de audio configuradas.
}

void loop(){
  if (aac->isRunning()) {//Verifica si el generador de audio AAC está en funcionamiento.
    aac->loop();//Si el generador de audio está en funcionamiento, se ejecuta el bucle de generación de audio.
  } else {
    aac -> stop();//Detiene la generación de audio AAC
    Serial.printf("Sound Generator\n");//mensaje en el monitor serie indicando que el generador de sonido se ha detenido
    delay(1000);//Espera un segundo antes de reiniciar el bucle
  }
}
```



