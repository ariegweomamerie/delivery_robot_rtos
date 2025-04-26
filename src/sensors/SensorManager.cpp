// In PathPlanner.cpp
void PathPlanner::_calculatePath(PathData_t data) {
    const float CRITICAL_DISTANCE = 20.0f;  // 20cm emergency stop
    const float WARNING_DISTANCE = 50.0f;    // 50cm slow down
    
    MotorCommand_t cmd;
    
    if (data.obstacle_distance < CRITICAL_DISTANCE) {
        cmd = {.command = EMERGENCY_STOP, .speed = 0};
    } 
    else if (data.obstacle_distance < WARNING_DISTANCE) {
        // Proportional speed reduction
        uint8_t speed = (uint8_t)(100.0f * (data.obstacle_distance - CRITICAL_DISTANCE) 
                         / (WARNING_DISTANCE - CRITICAL_DISTANCE));
        cmd = {.command = MOVE_FORWARD, .speed = speed};
    } 
    else {
        // Normal navigation with A* path planning (simplified)
        cmd = {.command = MOVE_FORWARD, .speed = 100};
    }
    
    xQueueSend(_motor_queue, &cmd, portMAX_DELAY);
}