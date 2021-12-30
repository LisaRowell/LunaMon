#include <Arduino.h>

#include "NMEADataValid.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"

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

void NMEADataValid::publish(DataModelLeaf &leaf) {
    if (valid) {
        leaf << "1";
    } else {
        leaf << "0";
    }
}

void NMEADataValid::log(Logger &logger) const {
    if (valid) {
        logger << "Valid";
    } else {
        logger << "Invalid";
    }
}
