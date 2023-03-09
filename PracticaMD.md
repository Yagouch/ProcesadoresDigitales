# Codigo #
```
#include <Arduino.h>

/* this function will be invoked when additionalTask was created */
void anotherTask( void * parameter )
{
  /* loop forever */
  for(;;)
  {
  Serial.println("this is another Task");
  delay(1000);
  }
  /* delete a task when finish,
  this will never happen because this is infinity loop */
  vTaskDelete( NULL );
}

void setup()
{
  Serial.begin(112500);
  /* we create a new task here */
  xTaskCreate(
  anotherTask, /* Task function. */
  "another Task", /* name of task. */
  10000, /* Stack size of task */
  NULL, /* parameter of the task */
  1, /* priority of the task */
  NULL); /* Task handle to keep track of created task */ 
}
 
/* the forever loop() function is invoked by Arduino ESP32 loopTask */
void loop() 
{
  Serial.println("this is ESP32 Task");
  delay(1000);
}
```

## Salida ##

```
[...]
this is another Task
this is ESP32 Task
this is another Task
this is ESP32 Task
[...]
```

# Codigo 2 - LED #
```
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
        
        vTaskDelay(pdMS_TO_TICKS(00));
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

void loop() { 
  delay(1000);
}
```

# Codigo 2 - 2 leds #
```
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