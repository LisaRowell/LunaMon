#include <Arduino.h>

#include "NMEADataValid.h"

bool parseNMEADataValid(String &validStr, bool &valid) {
    if (validStr == "A") {
        valid = true;
        return true;
    } else if (validStr == "V") {
        valid = false;
        return true;
    } else {
        return false;
    }
}

void printNMEADataValid(bool dataValid) {
    if (dataValid) {
        Serial.print("Valid");
    } else {
        Serial.print("Invalid");
    }
}