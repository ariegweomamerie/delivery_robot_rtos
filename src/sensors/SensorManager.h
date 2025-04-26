#include "PathPlanner.h"

class SensorManager {
public: 
    void registerPathPlanner(PathPlanner* planner) {
        _planner = planner:
    }


    void updateSensors() {
        PathData_t data = {
            .obstacle_distance = getUltrasonicDistance(),
            .target_x = current_target.x,
            .target_y = _current_target.y
        };
        if (_planner) xQueueSend(_planner->getSensorQueue(), &data, 0);
    }
private: 
    PathPlanner* _planner = nullptr;
};



/** The PathPlanner class is the "brain" of your autonomous robot's navigation system. It uses FreeRTOS to make real-time decisions about how the robot should move based on sensor data and destination goals. Here's a detailed breakdown:

Core Responsibilities
Obstacle Avoidance

Continuously checks ultrasonic/infrared sensor data (distance to obstacles)

Triggers emergency stops if obstacles are too close (e.g., <30cm)

Adjusts speed proportionally in tight spaces

Path Calculation

Processes destination coordinates (target_x, target_y)

Determines optimal movement commands (forward/backward/turns)

Can be extended with pathfinding algorithms (A, RRT)

Real-Time Coordination

Communicates with other FreeRTOS tasks via queues: */