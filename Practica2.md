# Informe - Practica 2 #
***
En esta práctica vamos a trabajar con interrupciones (ISH) las cuales, como su nombre indica son interrupciones de la actividad normal del procesador, para ejecutar otra actividad (y posteriormente reanudar la actividad normal).

Para ello vamos a usar dos tipos de eventos los cuales ejecutarán una función (ISR). Estos eventos son:
* Evento hardware (Boton).
* Evento programado (Timer).


## Interrupcion por hardware - Parte A ##
***
Lo primero que debemos hacer es declarar el evento por hardware que deseemos implementar, en este caso un botón.
Declaramos una estructura que almacene el pin que vamos a asignar a la interrupción, una variable que almacene el número de veces que activamos el boton, y una variable booleana que indique si el botón esta activado.

```
struct Button {
const uint8_t PIN;
uint32_t numberKeyPresses;
bool pressed;
};
```
Para las dos primeras variables usaremos los tipos definidos uintX_t, el cual será un int sin signo de X bits.

Para continuar usamos la estructura creada para definir el pin GPIO 18 como la entrada para nuestro botón.
```
Button button1 = {18, 0, false};
```
Despues declaramos la función que se ejecutará cuando la interrupción se lleve a cabo, la cual, simplemente, sumará uno a las veces que hemos pulsado el botón, y pondra el booleano a 1.
```
void IRAM_ATTR isr() {
button1.numberKeyPresses += 1;
button1.pressed = true;
}
```
En el ```void setup()``` aparte de declarar la comunicación serie con ```Serial.begin()```, vamos a declarar el pin que hemos usado para la interrupción por botón como un ```INPUT_PULLUP``` el cual es como un INPUT normal pero con una resistencia en *pullup*, para conseguir que mientras no se pulse el botón, la salida de un nivel alto de tensión continuo y no haya rebotes ocasionados por el comportamiento normal del botón.

Además incluiremos lo mas importante para esta práctica, el ```attachInterrupt(button1.PIN, isr, FALLING);```, que es el encargado de vinvular el pin con la funcion, y lo pondremos en modo *FALLING* para que cuando el boton se pulse (pasa de HIGH a LOW) se ejecute la interrupcion.

En el ```void loop()``` escribimos un *if* para que cuando la interrupcion se acabe el *if* se active y retorne el booleano a false e imprima por pantalla el número de veces que se ha activado el boton, además habrá otro *if* que se encargá de que cuando pase 1 minuto (60000 ms), ejecute el ```detachInterrupt(button1.PIN);``` e imprima 'Interrupt Detached!' para que haga una separación de interrupciones cuando ya no se quiera que el ESP32 monitorice el pin.


### Codigo ###
***
```
#include <Arduino.h>

struct Button {
const uint8_t PIN;
uint32_t numberKeyPresses;
bool pressed;
};

Button button1 = {18, 0, false};

void IRAM_ATTR isr() {
button1.numberKeyPresses += 1;
button1.pressed = true;
}


static uint32_t lastMillis = 0;

void setup() {

Serial.begin(115200);
pinMode(button1.PIN, INPUT_PULLUP);
attachInterrupt(button1.PIN, isr, FALLING);

}

void loop() {

if (button1.pressed) {
  Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
  button1.pressed = false;
}

//Detach Interrupt after 1 Minute
if (millis() - lastMillis > 60000) {
  lastMillis = millis();
  detachInterrupt(button1.PIN);
  Serial.println("Interrupt Detached!");
}

}
```
### Salida del código ###
***
```
[...]
Button 1 has been pressed 8701 times
Button 1 has been pressed 8710 times
Button 1 has been pressed 8713 times
Button 1 has been pressed 8715 times
Button 1 has been pressed 8716 times
Interrupt Detached!
```

## Interrupcion por Timer - Parte B ##
***



### Codigo ###
***
```
#include <Arduino.h>

volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
};

void setup() {

  Serial.begin(115200);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);

}

void loop() {
  
  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    totalInterruptCounter++;
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
  }

}
```

### Salida del código ###
***
```
An interrupt as occurred. Total number: 1
An interrupt as occurred. Total number: 2
An interrupt as occurred. Total number: 3
An interrupt as occurred. Total number: 4
An interrupt as occurred. Total number: 5
[...]
```
