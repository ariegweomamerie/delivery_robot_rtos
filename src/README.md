🤖 Project Title: Autonomous Obstacle-Avoiding Delivery Robot
🔧 System Overview
An autonomous mobile robot that:

Navigates an indoor environment.

Avoids obstacles using ultrasonic and infrared sensors.

Delivers payloads from a start to a destination point.

Supports task concurrency like navigation, obstacle detection, motor control, telemetry, and sensor fusion.




🧱 Core FreeRTOS Features Used

Feature	                   Use Case
Multitasking	           Navigation, sensors, motor control, telemetry run in parallel

Real-Time Scheduling	   Prioritize critical tasks like obstacle avoidance

Queues	                   Sensor data passed to decision logic or telemetry task

Mutexes	                   Shared access to motors or I2C bus

Semaphores	               Sync between sensors and control logic





🧩 Task Breakdown

Task	            Priority	Description
SensorTask	        Medium	    Polls distance sensors and sends data to a queue
MotorControlTask	High	    Reads decisions and drives motors
PathPlanningTask	Medium	    Calculates path based on sensor data
TelemetryTask	    Low	        Sends data via UART/Bluetooth
LEDStatusTask	    Low	        Updates robot status LEDs
WatchdogTask	    Highest	    Ensures system health and resets if hung


