#include "NMEAUInt8.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

#include <Arduino.h>

bool NMEAUInt8::set(const String &valueStr, bool optional, uint8_t maxValue) {
    const unsigned length = valueStr.length();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    if (!extractUInt8FromString(valueStr, 0, length, value, maxValue)) {
        return false;
    }

    valuePresent = true;
    return true;
}

bool NMEAUInt8::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                        const char *fieldName, bool optional, uint8_t maxValue) {
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

    if (!set(decimalStr, optional, maxValue)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << decimalStr << "'" << eol;
        return false;
    }

    return true;
}

bool NMEAUInt8::hasValue() const {
    return valuePresent;
}

void NMEAUInt8::publish(DataModelLeaf &leaf) const {
    if (valuePresent) {
        leaf << value;
    } else {
        leaf << "";
    }
}

void NMEAUInt8::log(Logger &logger) const {
    if (valuePresent) {
        logger << value;
    } else {
        logger << "NA";
    }
}
