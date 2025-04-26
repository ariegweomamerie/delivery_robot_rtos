#include "Telemetry.h"
#include <Arduino.h>

Telemetry::Telemetry() {
    Serial.begin(115200);
}


void Telemetry::sendStatus() {
    Serial.printIn("Sending telemetry data...");
}