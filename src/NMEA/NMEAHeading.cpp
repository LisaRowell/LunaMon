#include <Arduino.h>

#include "NMEAHeading.h"

#include "Util/CharacterTools.h"
#include "Util/StringTools.h"
#include "Util/Logger.h"

bool NMEAHeading::set(const String &headingStr) {
    const unsigned length = headingStr.length();
    if (length == 0) {
        hasValue = false;
        return true;
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

    hasValue = true;
    return true;
}

bool NMEAHeading::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String headingStr;
    if (!nmeaLine.extractWord(headingStr)) {
        logger << logWarning << talker << " " << msgType << " message missing Heading field" << eol;
        return false;
    }

    if (!set(headingStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad Heading field '"
               << headingStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEAHeading::publish(DataModelLeaf &leaf) const {
    if (hasValue) {
        char headingStr[10];

        snprintf(headingStr, 10, "%u.%u", heading, tenths);
        leaf << headingStr;
    } else {
        leaf << "";
    }
}

void NMEAHeading::log(Logger &logger) const {
    if (hasValue) {
        logger << heading << "." << tenths << "\xC2\xB0";
    } else {
        logger << "Unknown\xC2\xB0";
    }
}
