#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Pin definitions
#define FRONT_TRIG_PIN 2
#define FRONT_ECHO_PIN 3
#define LEFT_TRIG_PIN 4
#define LEFT_ECHO_PIN 5
#define RIGHT_TRIG_PIN 6
#define RIGHT_ECHO_PIN 7

// Threshold distance in cm
#define MOVE_THRESHOLD 5.0f

// Queue handle
QueueHandle_t xUltrasonicQueue;

typedef enum {
    SENSOR_FRONT,
    SENSOR_LEFT,
    SENSOR_RIGHT
} SensorType;

typedef struct {
    SensorType sensor;
    float distance_cm;
} UltrasonicData;

// Shared ultrasonic task function
void vUltrasonicTask(void *pvParameters) {
    SensorType sensor_type = (SensorType)pvParameters;
    uint trig_pin, echo_pin;

    // Configure pins based on sensor type
    switch(sensor_type) {
        case SENSOR_FRONT:
            trig_pin = FRONT_TRIG_PIN;
            echo_pin = FRONT_ECHO_PIN;
            break;
        case SENSOR_LEFT:
            trig_pin = LEFT_TRIG_PIN;
            echo_pin = LEFT_ECHO_PIN;
            break;
        case SENSOR_RIGHT:
            trig_pin = RIGHT_TRIG_PIN;
            echo_pin = RIGHT_ECHO_PIN;
            break;
    }

    gpio_init(trig_pin);
    gpio_set_dir(trig_pin, GPIO_OUT);
    gpio_init(echo_pin);
    gpio_set_dir(echo_pin, GPIO_IN);

    UltrasonicData xData;
    xData.sensor = sensor_type;

    while(1) {
        // Trigger pulse
        gpio_put(trig_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(2));
        gpio_put(trig_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_put(trig_pin, 0);

        // Measure echo
        uint32_t start = 0, end = 0;
        while(gpio_get(echo_pin) == 0) start = time_us_32();
        while(gpio_get(echo_pin) == 1) end = time_us_32();

        xData.distance_cm = (end - start) * 0.034f / 2.0f;
        xQueueSend(xUltrasonicQueue, &xData, portMAX_DELAY);  // the portMAX_DELAY parameter 
                                                              //is crucial for controlling 
                                                              //how the task behaves when the queue is full.

        
        vTaskDelay(pdMS_TO_TICKS(50)); // 20Hz update rate
    }
}

// Processing task with movement logic
void vMovementTask(void *pvParameters) {
    UltrasonicData xData;
    float front_dist = 0, left_dist = 0, right_dist = 0;
    
    while(1) {
        if(xQueueReceive(xUltrasonicQueue, &xData, portMAX_DELAY) == pdPASS) {
            // Update the appropriate distance
            switch(xData.sensor) {
                case SENSOR_FRONT: front_dist = xData.distance_cm; break;
                case SENSOR_LEFT: left_dist = xData.distance_cm; break;
                case SENSOR_RIGHT: right_dist = xData.distance_cm; break;
            }
            
            // Implement movement logic
            if(left_dist > MOVE_THRESHOLD) {
                printf("Robot moving left (L:%.1fcm, F:%.1fcm, R:%.1fcm)\n",
                      left_dist, front_dist, right_dist);
            }
            else if(right_dist > MOVE_THRESHOLD) {
                printf("Robot moving right (L:%.1fcm, F:%.1fcm, R:%.1fcm)\n",
                      left_dist, front_dist, right_dist);
            }
            else {
                printf("Robot moving forward (L:%.1fcm, F:%.1fcm, R:%.1fcm)\n",
                      left_dist, front_dist, right_dist);
            }
        }
    }
}

int main() {
    stdio_init_all();
    
    // Create queue to hold up to 10 readings
    xUltrasonicQueue = xQueueCreate(10, sizeof(UltrasonicData));
    
    // Create sensor tasks
    xTaskCreate(vUltrasonicTask, "FrontUS", 256, (void*)SENSOR_FRONT, 2, NULL);
    xTaskCreate(vUltrasonicTask, "LeftUS", 256, (void*)SENSOR_LEFT, 2, NULL);
    xTaskCreate(vUltrasonicTask, "RightUS", 256, (void*)SENSOR_RIGHT, 2, NULL);
    
    // Create movement task
    xTaskCreate(vMovementTask, "Movement", 512, NULL, 1, NULL);
    
    vTaskStartScheduler();
    
    while(1) tight_loop_contents();
}