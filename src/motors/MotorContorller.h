#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H



#include "FreeRTOS.h"
#include "semphr.h"
#include "hardware/pwm.h"


class MotorController {
    public:
       MotorController(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t ena, uint_8_t enb);
       
       BaseType_t forward(uint8_t speed = 100, TickType_t timeout = pdMS_TO_TICKS(100));
       BaseType_t backward(uint8_t speed = 100, TickType_t timeout = pdMs_TO_TICKS(100));
       BaseType_t stop(TickType_t timeout = pdMS_TO_TICKS(100));
       BaseType_t setSpeed(uint8_t speed, TickType_t timeout = pdMS_TO_TICKS(100));

    
    private: 
       uint8_t _pin1, _pin2, _pin3, _pin4;  // L298N direction pins
       uint8_t _ena, _enb;                  // PWM speed control pins
       uint _ena_slice, _enb_slice;         // RP2024 PWM slices
       SemaphoreHandle_t _mutex;            // FreeROTS mutex for thread safety


      void _initPWM(uint8_t pin, uint* slice);
};

#endif 