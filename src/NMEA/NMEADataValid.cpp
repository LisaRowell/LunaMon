#include <Arduino.h>

#include "NMEADataValid.h"

#include "DataModel/DataModelLeaf.h"

NMEADataValid::NMEADataValid() : valid(false) {
}

bool NMEADataValid::set(String &validStr) {
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

void NMEADataValid::print() {
    if (valid) {
        Serial.print("Valid");
    } else {
        Serial.print("Invalid");
    }
}

void NMEADataValid::publish(DataModelLeaf &leaf) {
    if (valid) {
        leaf << "1";
    } else {
        leaf << "0";
    }
}