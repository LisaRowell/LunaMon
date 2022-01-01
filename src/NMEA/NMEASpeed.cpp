#include <Arduino.h>

#include "NMEASpeed.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

bool NMEASpeed::set(const String &speedStr) {
    const unsigned length = speedStr.length();
    if (length == 0) {
        hasValue = false;
        return true;
    }

    int periodPos = speedStr.indexOf(".");
    if (periodPos > 0) {
        if (!extractUInt16FromString(speedStr, 0, periodPos, speed)) {
            return false;
        }

        if (length > (unsigned)periodPos + 1) {
            if (!extractDigitFromString(speedStr, periodPos + 1, tenths)) {
                return false;
            }
        }
    } else if (periodPos == 0) {
        speed = 0;
        if (length > 1) {
            if (!extractDigitFromString(speedStr, 1, tenths)) {
                return false;
            }
        }
    } else {
        if (!extractUInt16FromString(speedStr, 0, length, speed)) {
            return false;
        }
        tenths = 0;
    }

    hasValue = true;
    return true;
}

bool NMEASpeed::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String speedStr;
    if (!nmeaLine.extractWord(speedStr)) {
        logger << logWarning << talker << " " << msgType << " message missing Speed field" << eol;
        return false;
    }

    if (!set(speedStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad Speed field '"
               << speedStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEASpeed::publish(DataModelLeaf &leaf) const {
    if (hasValue) {
        char speedStr[10];

        snprintf(speedStr, 10, "%u.%u", speed, tenths);
        leaf << speedStr;
    } else {
        leaf << "";
    }
}

void NMEASpeed::log(Logger &logger) const {
    if (hasValue) {
        logger << speed << "." << tenths << "kn";
    } else {
        logger << "Unknown kns";
    }
}
