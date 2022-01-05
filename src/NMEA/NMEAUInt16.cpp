#include <Arduino.h>

#include "NMEAUInt16.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"
#include "Util/Error.h"

bool NMEAUInt16::set(const String &valueStr, bool optional) {
    const unsigned length = valueStr.length();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    if (!extractUInt16FromString(valueStr, 0, length, value)) {
        return false;
    }

    valuePresent = true;
    return true;
}

bool NMEAUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                         const char *fieldName, bool optional, uint16_t maxValue) {
    String decimalStr;
    if (!nmeaLine.extractWord(decimalStr)) {
        if (!optional) {
            logger << logWarning << talker << " " << msgType << " message missing " << fieldName
                   << " field" << eol;
            return false;
        }

        valuePresent = false;
        return true;
    }

    if (!set(decimalStr, optional)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << decimalStr << "'" << eol;
        return false;
    }

    return true;
}

bool NMEAUInt16::hasValue() const {
    return valuePresent;
}

uint16_t NMEAUInt16::getValue() const {
    if (!valuePresent) {
        fatalError("Attempt to read value from NMEAUInt8 with value not present");
    }

    return value;
}

void NMEAUInt16::publish(DataModelLeaf &leaf) const {
    if (valuePresent) {
        leaf << value;
    } else {
        leaf << "";
    }
}

void NMEAUInt16::log(Logger &logger) const {
    if (valuePresent) {
        logger << value;
    } else {
        logger << "NA";
    }
}
