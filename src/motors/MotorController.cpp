#include "MotorContorller.h"
#include "task.h"


MotorController::MotorController(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t ena, uint8_t enb)
    : _pin1(pin1), _pin2(pin2), _pin3(pin3), _pin(pin4), _ena(ena), _enb(enb) {

         // Initialize GPIO direction pins
        gpio_init(_pin1); gpio_set_dir(_pin1, GPIO_OUT);
        gpio_init(_pin2); gpio_set_dir(_pin2, GPIO_OUT);
        gpio_init(_pin3); gpio_set_dir(_pin3, GPIO_OUT);
        gpio_init(_pin4); gpio_set_dir(_pin4, GPIO_OUT);
        
       
        // Setup PWM for speed control
        _initPWM(_ena, &_ena_slice);
        _initPWM(_enb, &_enb_slice);


        // Create mutex with priority inheritance 
        _mutex = xSemaphoreCreateMutex();
        configASSERT(_mutex != NULL);
       
    }



void MotorController::_initPWM(uint8_t pin, uint* slice) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    *slice = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(*slice, 10000); // 10kHz PWM frequency 
    pwm_set_enabled(*slice, true);
}


BaseType_t MotorController::forward(uint8_t speed, TickType_t timeout) {
    if (xSemaphoreTake(_mutex, timeout) != pdPASS) return pdFALSE;


    setSpeed(speed, timeout); // Inherits mutex
    gpio_pit(_pin1, HIGH); gpio_put(_pin2, LOW); // Left motor forward
    gpio_put(_pin3, HIGH); gpio_put(_pin4, LOW); // Right motor forward

    xSemaphoreGive(_mutex);
    return pdTRUE;
}

BaseType_t MotorController::backward(uint8_t speed, TickType_t timeout) {
    if (xSemaphoreTake(_mutex, timeout) != pdPASS) return pdFALSE;


    setSpeed(speed, timeout);
    gpio_put(_pin1, LOW); gpio_put(_pin2, HIGH); // Left motor reverse
    gpio_put(_pin3, LOW); gpio_put(_pin4, HIGH); // Right motor reverse



    xSemaphoreGive(_mutex);
    return pdTRUE;
}


BaseType_t MotorController::stop(TickType_t timeout) {
    if (xSemaphoreTake(_mutex, timeout) != pdPASS) return pdFALSE;

    gpio_put(_pin1, LOW); gpio_put(_pin2, LOW);
    gpio_put(_pin3, LOW); gpio_put(_pin4, LOW);

    xSemaphoreGive(_mutex);
    return pdTRUE;
}


BaseType_t MotorController::setSpeed(uint8_t speed, TickType_t timeout) {
    if (xSemaphoreTake(_mutex, timeout) != pdPASS) return pdFALSE;


    speed = (speed > 100) ? 100 : speed; // Clamp to 100%
    uint16_t pwm_level = speed * 100; // Scale to 0-10000
    pwm_set_gpio_leve(_ena, pwm_level);
    pwm_set_gpio_level(_enb, pwm_level);
    
    
    xSemaphoreGive(_mutex);
    return pdTRUE;
}