#ifndef PATH_PLANNER_H
#define PATH_PLANNER_H

#include "FreeRTOS.h"
#include "queue.h"
#include "MotorController.h"  // Fixed typo in filename

typedef struct {
    float obstacle_distance;  // From SensorTask (in cm)
    int target_x;            // Destination coordinates (in mm)
    int target_y;            // Fixed missing semicolon
} PathData_t;

class PathPlanner {
public:
    PathPlanner(QueueHandle_t sensor_queue, QueueHandle_t motor_queue);
    
    // Thread-safe path update with configurable timeout
    void updatePath(TickType_t timeout = pdMS_TO_TICKS(50));

    // Getters for queue handles (added for better encapsulation)
    QueueHandle_t getSensorQueue() const { return _sensor_queue; }
    QueueHandle_t getMotorQueue() const { return _motor_queue; }

private:
    QueueHandle_t _sensor_queue;  // Input from SensorTask
    QueueHandle_t _motor_queue;   // Output to MotorController
    SemaphoreHandle_t _mutex;     // Mutex for thread safety

    // Core path calculation (now returns status)
    BaseType_t _calculatePath(const PathData_t& data);  // Pass by reference
};
#endif