#include "NMEADirection.h"

#include "Util/CharacterTools.h"
#include "Util/StringTools.h"
#include "Util/Logger.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <Arduino.h>

using etl::string;
using etl::string_stream;

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

void NMEADirection::publish(DataModelTenthsUInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf.set(heading, tenths);
    } else {
        leaf.removeValue();
    }
}

void NMEADirection::log(Logger &logger) const {
    if (valuePresent) {
        logger << heading << "." << tenths << "\xC2\xB0";
    } else {
        logger << "Unknown\xC2\xB0";
    }
}
