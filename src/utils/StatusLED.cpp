#include "StatusLED.h"
#include <Arduino.h>


#define STATUS_LED_PIN 13


StatusLED::StatusLED() {
    pinMode(STATUS_LED_PIN, OUTPUT);
}

void StatusLED::blink() {
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
}