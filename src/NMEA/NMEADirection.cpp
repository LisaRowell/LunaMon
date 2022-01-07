#include <Arduino.h>

#include "NMEADirection.h"

#include "Util/CharacterTools.h"
#include "Util/StringTools.h"
#include "Util/Logger.h"

bool NMEADirection::set(const String &headingStr, bool optional) {
    const unsigned length = headingStr.length();
    if (length == 0) {
        if (optional) {
            valuePresent = false;
            return true;
        } else {
            return false;
        }
    }

    int periodPos = headingStr.indexOf(".");
    if (periodPos > 0) {
        if (!extractUInt16FromString(headingStr, 0, periodPos, heading)) {
            return false;
        }

        if (length > (unsigned)periodPos + 1) {
            if (!extractDigitFromString(headingStr, periodPos + 1, tenths)) {
                return false;
            }
        }
    } else if (periodPos == 0) {
        heading = 0;
        if (length > 1) {
            if (!extractDigitFromString(headingStr, 1, tenths)) {
                return false;
            }
        }
    } else {
        if (!extractUInt16FromString(headingStr, 0, length, heading)) {
            return false;
        }
        tenths = 0;
    }

    valuePresent = true;
    return true;
}

bool NMEADirection::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                            const char *fieldName, bool optional) {
    String directionStr;
    if (!nmeaLine.extractWord(directionStr)) {
        if (!optional) {
            logger << logWarning << talker << " " << msgType << " message missing " << fieldName
                   << " field" << eol;
            return false;
        }

        valuePresent = false;
        return true;
    }

    if (!set(directionStr, optional)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '"<< directionStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEADirection::publish(DataModelLeaf &leaf) const {
    if (valuePresent) {
        char headingStr[10];

        snprintf(headingStr, 10, "%u.%u", heading, tenths);
        leaf << headingStr;
    } else {
        leaf << "";
    }
}

void NMEADirection::log(Logger &logger) const {
    if (valuePresent) {
        logger << heading << "." << tenths << "\xC2\xB0";
    } else {
        logger << "Unknown\xC2\xB0";
    }
}
