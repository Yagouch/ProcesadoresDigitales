# Informe - Práctica 3 #
***
En esta práctica gracias al uso del **Sistema Operativo a Tiempo Real** *FreeRTOS*, el cual nos permite la creación de procesos/tareas idependientes, vamos a aprender el uso de las **tasks** para que nuestro porcesador sea capaz de ejecutar varias acciones en un tiempo reducido. Además veremos **los semáforos y los mutex** para poder manipular estas tareas a voluntad o de forma automatizada. Nuestro objetivo es que usando las colas podremos ejecutar unas acciones entre los delays de otras, con el resultado de que para la percepción humana se estén ejecutando varios procesos a la vez.

## Creación de *Tasks* - Parte A ##
***
En este primer código vamos a crear una *Task* que se ejecutará en el delay del ```void loop()```. En el ```void setup()``` crearemos la tarea con el ``` xTaskDelete() ``` donde colocaré los parámetros: ``` Task_Function, Task_Name, Stack_size, Parameter, Task_Priority, Task_Handle ```, donde:

```Task_function```: Es el nombre de la función que se ejecutará cuando llamemos a la tarea.
```Task_Name```: Es el nombre de la tarea.
```Stack_size```: Es el tamaño de palabras (agrupación de bytes) que desdicamos a la ejecución de la tarea.
```Parameter```: Es un valor que es enviado como parámetro a la tarea.
```Task_Priority```: Es la prioridad a la que se ejecutará la tarea. (Cuando se quieren ejecutar dos tareas a la vez, se ejecutará la de mayor prioridad mientras que la otra se pone en la cola)
```Task_Handle```: Es para mantener el trastro de la tarea creada.

Anteriormente hemos creado la función que se ejecutará con esta Tarea, la cual hemos llamado ```anotherTask()```, que recibe el parámetro, que en nuestro caso es NULL, como un puntero genérico. En esta función se creará un bucle infinito con ```for(;;){}```, dentro imprimimos por pantalla que tarea es, para ver como se van intercalando las tareas y un delay, para que se puedan diferenciar las tareas correctamente por pantalla. Además fuera del bucle infinito pondremos un ```vTaskdelete(NULL)``` para acabar la tarea, aunque nunca pasará porque antes hay un loop infinito, siempre está bien ponerlo para evitar posibles errores.

Finalmente, en el ```void loop()``` imprimiremos por pantalla algo para que se vea por pantalla como se van ejecutando intercaladamente la tarea y el loop principal. Con su respectivo delay para poder ver por pantalla como ambas tareas se van intercalando.


### Código A ###
***
```cpp
#include <Arduino.h>

void anotherTask( void * parameter )
{
  for(;;){

  Serial.println("this is another Task");
  delay(1000);

  }
  vTaskDelete( NULL );
}

void setup()
{
  Serial.begin(112500);

  xTaskCreate(
  anotherTask,
  "another Task",
  10000,
  NULL,
  1, 
  NULL);
}
 

void loop() 
{
  Serial.println("this is ESP32 Task");
  delay(1000);
}
```

### Salida del código A ###
***
```
[...]
this is another Task
this is ESP32 Task
this is another Task
this is ESP32 Task
[...]
```
### Diagrama de flujo A ###
***
```
mermaid

```
***
***
## Creación de Semáforo Mutex - Parte B ##
***

Primero incluimos las librearias necesarias para usar los semáforos del *FreeRTOS*. A continuación se crea la variable ```mutex_v``` de tipo ```SemaphoreHandle_t``` para almacenar el semáforo que queremos crear.

En el ```void setup()```, se iniciará la comunicación serie con el esp32 seleccionando la tasa de transferencia (baud rate) correspondiente. Después, aparte del ```pinMode(32,OUTPUT)``` para declarar el pin 32 como pin de salida para ver con un LED como se van alternando las tareas, deberemos crear el mutex y almacenarlo en la variable de tipo semáforo que hemos creado antes ```mutex_v = xSemaphoreCreateMutex();```. A continuación si no se ha creado el mutex correctamente se ha de comunicar por pantalla al usuario con: ```if (mutex_v == NULL) {}```, y finalmente se crearán las tareas 1 y 2: ``` xTaskCreate(Task1, "Task1", 1000, NULL, 1, NULL);``` y ```xTaskCreate(Task2, "Task2", 1000, NULL, 1, NULL); ```.

Estas tareas, cuando se ejecuten, harán una llamada a la acción de una función que habrá que crear. Esta función se creará de la siguiente manera: ```void Task1(void *pvParameters){}```, será de tipo void y aunque no va a recibir nada (Cuarto parámetro del xTaskCreate=NULL), se incluirá un puntero de tipo void llamado *pvParameters*. Y dentro de esta función, la tarea tomará el protagonismo del mutex con un *Take*: ```xSemaphoreTake(mutex_v, portMAX_DELAY);```, donde habrá un bucle infinito ```while(1)``` que escribirá por pantalla que tarea es: ```Serial.println("Hi from Task1"); ```, cambiará el estado del LED con: ```digitalWrite(32,HIGH);``` y habrá un delay de la tarea de 0.1s: ```vTaskDelay(pdMS_TO_TICKS(100));```, para posteriormente ceder el semáforo a la siguiente tarea de la cola: ```xSemaphoreGive(mutex_v); ```, finalmente habrá otro delay de medio segundo ```vTaskDelay(pdMS_TO_TICKS(500));``` para que la tarea se espere un poco antes de coger el semáforo nuevamente.

Y lo mismo pasa con la función de la otra tarea. Una tarea encenderá el led y otra lo apagará siempre escribiendo por pantalla qué tarea se ha ejecutado.

En el código de dos LEDs hace lo mismo pero con otro LED de forma antagónica. La primera tarea Enciende un LED y apaga otro, y la segunda tarea apaga el que estaba encendido y enciende el apagado.

### Codigo B - 1 LED ###
***
```cpp
#include <Arduino.h>
//#include <Arduino_FreeRTOS.h> 
//#include <semphr.h> 

//#define portDELAY(TickType_t) 0x15UL

SemaphoreHandle_t mutex_v; 

void Task1(void *pvParameters);
void Task2(void *pvParameters);


void setup() { 
    Serial.begin(115200); 
    pinMode(32,OUTPUT);
    mutex_v = xSemaphoreCreateMutex(); 
    if (mutex_v == NULL) { 
        Serial.println("Mutex can not be created"); 
    } 
    xTaskCreate(Task1, "Task1", 1000, NULL, 1, NULL); 
    xTaskCreate(Task2, "Task2", 1000, NULL, 1, NULL); 
} 

void loop() { 
  delay(1000);
}

void Task1(void *pvParameters) { 
    while(1) { 
        xSemaphoreTake(mutex_v, portMAX_DELAY); 
        Serial.println("Hi from Task1"); 
        digitalWrite(32,HIGH);
        
        vTaskDelay(pdMS_TO_TICKS(100));
        xSemaphoreGive(mutex_v); 
        vTaskDelay(pdMS_TO_TICKS(500));
    } 
} 

void Task2(void *pvParameters) { 
   while(1) { 
       xSemaphoreTake(mutex_v, portMAX_DELAY); 
       Serial.println("Hi from Task2"); 
       digitalWrite(32,LOW);
       
       vTaskDelay(pdMS_TO_TICKS(100)); 
       xSemaphoreGive(mutex_v); 
       vTaskDelay(pdMS_TO_TICKS(500)); 
    } 
}

```

### Codigo 2 - 2 leds ###
***
```cpp
#include <Arduino.h>
//#include <Arduino_FreeRTOS.h> 
//#include <semphr.h> 

#define portDELAY(TickType_t) 0x15UL

SemaphoreHandle_t mutex_v; 

void Task1(void *pvParameters);
void Task2(void *pvParameters);


void setup() { 
    Serial.begin(115200); 
    pinMode(32,OUTPUT);
    pinMode(33,OUTPUT);
    mutex_v = xSemaphoreCreateMutex(); 
    if (mutex_v == NULL) { 
        Serial.println("Mutex can not be created"); 
    } 
    xTaskCreate(Task1, "Task1", 1000, NULL, 1, NULL); 
    xTaskCreate(Task2, "Task2", 1000, NULL, 1, NULL); 
} 

void Task1(void *pvParameters) { 
    while(1) { 
        xSemaphoreTake(mutex_v, portMAX_DELAY); 
        Serial.println("Hi from Task1"); 
        digitalWrite(32,HIGH);
        digitalWrite(33,LOW);
        
        vTaskDelay(pdMS_TO_TICKS(500));
        xSemaphoreGive(mutex_v); 
        vTaskDelay(pdMS_TO_TICKS(500));
    } 
} 

void Task2(void *pvParameters) { 
   while(1) { 
       xSemaphoreTake(mutex_v, portMAX_DELAY); 
       Serial.println("Hi from Task2"); 
       digitalWrite(32,LOW);
       digitalWrite(33,HIGH);
       
       vTaskDelay(pdMS_TO_TICKS(500)); 
       xSemaphoreGive(mutex_v); 
       vTaskDelay(pdMS_TO_TICKS(500)); 
    } 
}

void loop() { 
  delay(1000);
}
```

### Salida del código A ###
***
```
[...]
Hi from Task1
Hi from Task2
Hi from Task1
Hi from Task2
[...]
```

### Diagrama de flujo B ###
***
```
mermaid
```