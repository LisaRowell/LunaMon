#include "NMEADataValid.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelUInt32Leaf.h"

#include "Util/Logger.h"

#include <Arduino.h>

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

bool NMEADataValid::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String dataValidStr;
    if (!nmeaLine.extractWord(dataValidStr)) {
        logger << logWarning << talker << " " << msgType << " message missing Data Valid field"
               << eol;
        return false;
    }
    if (!set(dataValidStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad Data Valid field '"
               << dataValidStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEADataValid::publish(DataModelUInt32Leaf &leaf) const {
    if (valid) {
        leaf = 1;
    } else {
        leaf = 0;
    }
}

void NMEADataValid::log(Logger &logger) const {
    if (valid) {
        logger << "Valid";
    } else {
        logger << "Invalid";
    }
}
