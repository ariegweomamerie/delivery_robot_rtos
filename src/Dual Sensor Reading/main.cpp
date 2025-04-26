#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TRIG_PIN 2
#define ECHO_PIN 3
#define IR_SENSOR_PIN 26  


// Queue handles
QueueHandle_t xUltrasonicQueue;
QueueHandle_t xIRQueue;

  // My Data structures
typedef struct {
   float distance_cm;
   TickType_t timestamp;
} UltrasonicData;


typedef struct {
    uint16_t raw_value;
    float distance_cm;
    TickType_t timestamp;
} IRData;


// Ultrasonic sensor task 
void vUltrasonicTask(void *pvParameters) {
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);


    UltrasonicData xData;

    while (1) {
        // Send trigger pulse
        gpio_put(TRIG_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(2));
        gpio_put(TRIG_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_put(TRIG_PIN, 0)


        // Measure echo pluse width
        uint32_t start_time = 0;
        uint32_t end_time = 0;


        // wait for echo to go high 
        while (gpio_get(ECHO_PIN) == 0) {
            start_time = time_us_32();
        }


        // Calculate distance
        uint32_t pluse_duration = end_time - start_time;
        xData.distance_cm = pulse_duration * 0.034 / 2;
        xData.timestamp = xTaskGetTickCount();


        // Send to queue 
        xQueueSend(xUltrasonicQueue, &xData, portMAX_DELAY)  // the portMAX_DELAY parameter is crucial for controlling how the task behaves when the queue is full.


        // Delay between readings
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


// IR sensor task 
void vIRTask(void *pvParameters) {
    adc_init();
    adc_gpio_init(IR_SENSOR_PIN);
    adc_select_input(0);  


    IRData xData;
    const float conversion_factor = 3.3f / (1 << 12);


    while (1)
    {
        // Read ADC value 
        xData.raw_value = adc_read();


        // Convert to voltage 
        float voltage = xData.raw_value * conversion_factor;


        // Convert to distance (example for GP2Y0A21YK0F)
        // This is sensor-specific - adjust for your model

        if (voltage < 0.5) voltage = 0.5;
        xData.distance_cm = 13.0f * pow(voltage, -1.0f);


        xData.timestamp = xTaskGetTickCount();

        // Send to queue
        xQueueSend(xIRQueue, &xData, portMAX_DELAY);


        // Delay between readings
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    
}

// Function to get both sensor readings
void read_both_sensor(UltrasonicData *ultrasonic, IRData *ir) {
    if (xQueueReceive(xUltrasonicQueue, ultrasonic, 0) == pdPASS) {
         // Our Data received
    }

    if (xQueueReceive(xIRQueue, ir, 0) == pdPASS) {
        // Our Data received
    }
}


int main() {
    stdio_init_all();

    // Create queues
    xUltrasonicQueue = xQueueCreate(5, sizeof(UltrasonicData));
    xIRQueue = xQueueCreate(5, sizeof(IRData));

    // Create task 
    xTaskCreate(vUltrasonicTask, "Ultrasonic", 256, NULL, 2, NULL);
    xTaskCreate(vIRTask, "IR Sensor", 256, NUL, 2, NULL);


    // Start scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (1) {
        tight_loop_contents();
    }
}