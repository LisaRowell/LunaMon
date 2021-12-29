#include <Arduino.h>

#include "Error.h"

void fatalError(String errorMsg) {
    Serial.println(errorMsg);
    errorExit();
}

void errorExit() {
    Serial.flush();
    exit(1);
}
