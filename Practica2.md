# Practicia 2A #
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
### Impresion serie ###
```
Button 1 has been pressed 8465 times
Button 1 has been pressed 8466 times
Button 1 has been pressed 8482 times
Button 1 has been pressed 8484 times
Button 1 has been pressed 8490 times
Button 1 has been pressed 8497 times
Button 1 has been pressed 8582 times
Button 1 has been pressed 8583 times
Button 1 has been pressed 8584 times
Button 1 has been pressed 8586 times
Button 1 has been pressed 8589 times
Button 1 has been pressed 8663 times
Button 1 has been pressed 8664 times
Button 1 has been pressed 8665 times
Button 1 has been pressed 8668 times
Button 1 has been pressed 8674 times
Button 1 has been pressed 8691 times
Button 1 has been pressed 8692 times
Button 1 has been pressed 8695 times
Button 1 has been pressed 8698 times
Button 1 has been pressed 8701 times
Button 1 has been pressed 8710 times
Button 1 has been pressed 8713 times
Button 1 has been pressed 8715 times
Button 1 has been pressed 8716 times
Interrupt Detached!
Interrupt Detached!
```

# Practica 2B #
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

### Impresion serie ###
```
An interrupt as occurred. Total number: 1
An interrupt as occurred. Total number: 2
An interrupt as occurred. Total number: 3
An interrupt as occurred. Total number: 4
An interrupt as occurred. Total number: 5
An interrupt as occurred. Total number: 6
An interrupt as occurred. Total number: 7
An interrupt as occurred. Total number: 8
An interrupt as occurred. Total number: 9
An interrupt as occurred. Total number: 10
An interrupt as occurred. Total number: 11
An interrupt as occurred. Total number: 12
An interrupt as occurred. Total number: 13
```