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