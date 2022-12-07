#include "Error.h"

#include <etl/string.h>

#include <Arduino.h>

void fatalError(const char *errorMsg) {
    Serial.println(errorMsg);
    errorExit();
}

void fatalError(const etl::istring &errorMsg) {
    Serial.println(errorMsg.data());
    errorExit();
}

void errorExit() {
    Serial.flush();
    exit(1);
}
