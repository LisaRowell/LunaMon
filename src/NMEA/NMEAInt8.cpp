#include "NMEAInt8.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

#include <Arduino.h>

bool NMEAInt8::set(const String &decimalStr, bool optional, int8_t minValue, int8_t maxValue) {
    const unsigned length = decimalStr.length();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    if (!extractInt8FromString(decimalStr, 0, length, value, minValue, maxValue)) {
        return false;
    }

    return true;
}

bool NMEAInt8::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                       const char *fieldName, bool optional, int8_t minValue, int8_t maxValue) {
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

    if (!set(decimalStr, optional, minValue, maxValue)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << decimalStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEAInt8::publish(DataModelInt8Leaf &leaf) const {
    leaf = value;
}

void NMEAInt8::log(Logger &logger) const {
    logger << value;
}
