#include <Arduino.h>
//#include <Arduino_FreeRTOS.h> 
//#include <semphr.h> 

#define portDELAY(TickType_t) 0x15UL

SemaphoreHandle_t mutex_v; 

void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);
void Task4(void *pvParameters);



void setup() { 
    Serial.begin(115200); 
    pinMode(23,OUTPUT);
    pinMode(22,OUTPUT);
    pinMode(21,OUTPUT);
    pinMode(19,OUTPUT);
    mutex_v = xSemaphoreCreateMutex(); 
    if (mutex_v == NULL) { 
        Serial.println("Mutex can not be created"); 
    } 
    xTaskCreate(Task1, "Task1", 1000, NULL, 1, NULL); 
    xTaskCreate(Task2, "Task2", 1000, NULL, 1, NULL); 
    xTaskCreate(Task3, "Task3", 1000, NULL, 1, NULL); 
    xTaskCreate(Task4, "Task4", 1000, NULL, 1, NULL);
} 

void Task1(void *pvParameters) { 
    while(1) { 
        xSemaphoreTake(mutex_v, portMAX_DELAY); 
        Serial.println("Hi from Task1"); 
        digitalWrite(23,HIGH);
        digitalWrite(22,LOW);
        digitalWrite(21,LOW);
        digitalWrite(19,LOW);
        
        vTaskDelay(pdMS_TO_TICKS(200));
        xSemaphoreGive(mutex_v); 
        vTaskDelay(pdMS_TO_TICKS(600));
    } 
} 

void Task2(void *pvParameters) { 
   while(1) { 
       xSemaphoreTake(mutex_v, portMAX_DELAY); 
       Serial.println("Hi from Task2"); 
       digitalWrite(23,LOW);
       digitalWrite(22,HIGH);
       digitalWrite(21,LOW);
       digitalWrite(19,LOW);
       
       vTaskDelay(pdMS_TO_TICKS(200)); 
       xSemaphoreGive(mutex_v); 
       vTaskDelay(pdMS_TO_TICKS(600)); 
    } 
}
void Task3(void *pvParameters) { 
    while(1) { 
        xSemaphoreTake(mutex_v, portMAX_DELAY); 
        Serial.println("Hi from Task3"); 
        digitalWrite(23,LOW);
        digitalWrite(22,LOW);
        digitalWrite(21,HIGH);
        digitalWrite(19,LOW);
        
        vTaskDelay(pdMS_TO_TICKS(200));
        xSemaphoreGive(mutex_v); 
        vTaskDelay(pdMS_TO_TICKS(600));
    } 
} 
void Task4(void *pvParameters) { 
    while(1) { 
        xSemaphoreTake(mutex_v, portMAX_DELAY); 
        Serial.println("Hi from Task4"); 
        digitalWrite(23,LOW);
        digitalWrite(22,LOW);
        digitalWrite(21,LOW);
        digitalWrite(19,HIGH);
        
        vTaskDelay(pdMS_TO_TICKS(200));
        xSemaphoreGive(mutex_v); 
        vTaskDelay(pdMS_TO_TICKS(600));
    } 
} 

void loop() { 
  delay(1000);
}