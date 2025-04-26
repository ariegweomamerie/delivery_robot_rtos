#include "PathPlanner.h"
#include "task.h"

PathPlanner::PathPlanner(QueueHandle_t sensor_queue, QueueHandle_t motor_queue) 
   : _sensor_queue(sensor_queue), _motor_queue(motor_queue) {

    _mutex = xSemaphoreCreateMutex();
    configASSERT(_mutex != NULL);
   }
/**
 * The function `updatePath` in the `PathPlanner` class updates the path based on the latest sensor
 * data if available within a specified timeout.
 * 
 * @param timeout The `timeout` parameter in the `updatePath` function is of type `TickType_t`. This
 * parameter is used to specify the maximum amount of time the function should wait for data to be
 * received from the sensor queue before timing out. The `timeout` value is in ticks, which are
 * typically
 */

void PathPlanner::updatePath(TickType_t timeout) {
    PathData_t sensor-data;

    // 1. Get latest sensor data (non-blocking)
    if (xQueueReceive(_sensor_queue, &sensor_data, timeout) == pdPASS) {
        if (xSemaphoreTake(_mutex, timeout) == pdTRUE) {
            _calculatePath(sensor_queue);
            xSemaphoreGive(_mutex);
        }
    }
}


void PathPlanner::_calculatePath(PathData_t data) {
     // 2. Simple obstacle avoidance algorithm
     uint8_t new_speed = 100;
     if (data.obstacle_distance < 30.0f) { // 30 threshold
         new_speed = 0; // Stop if obstacle too close 

         // Send emergency stop command
         MotorCommand_t cmd = {.command = EMERGENCY_STOP};
         xQueueSend(_motor_queue, &cmd, 0);
     }

     
     // 3. Normal path planning (expand with your logic)
     MotorCommand_t cmd = {
        .command = MOVE_FORWARD,
        .speed = new_speed,
        .duration = pdMS_TO_TICKS(500)
     };
     xQueueSend(_motor_queue, &cmd, 0);
}