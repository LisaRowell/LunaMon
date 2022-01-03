#include <Arduino.h>

#include "NMEATenthsUInt16.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

bool NMEATenthsUInt16::set(const String &decimalStr, bool optional) {
    const unsigned length = decimalStr.length();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    const int periodPos = decimalStr.indexOf(".");
    if (periodPos != 0) {
        if (!extractUInt16FromString(decimalStr, 0, periodPos, wholeNumber)) {
            return false;
        }
    } else {
        wholeNumber = 0;
    }

    if (periodPos >= 0) {
        const unsigned charactersAfterPeriod = length - periodPos - 1;
        if (charactersAfterPeriod) {
            const unsigned decimalStart = periodPos + 1;
            const unsigned placesToRead = min((unsigned)2, charactersAfterPeriod);
            const unsigned decimalEnd = decimalStart + placesToRead;
            uint16_t decimalPortion;
            if (!extractUInt16FromString(decimalStr, decimalStart, decimalEnd, decimalPortion)) {
                return false;
            }
            if (placesToRead > 1) {
                tenths = (decimalPortion + 5) / 10;
            } else {
                tenths = decimalPortion;
            }
        } else {
            tenths = 0;
        }
    } else {
        tenths = 0;
    }

    valuePresent = true;
    return true;
}

bool NMEATenthsUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                               const char *fieldName, bool optional) {
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

bool NMEATenthsUInt16::hasValue() const {
    return valuePresent;
}

void NMEATenthsUInt16::publish(DataModelLeaf &leaf) const {
    if (valuePresent) {
        char decimalStr[11];

        snprintf(decimalStr, 11, "%u.%u", wholeNumber, tenths);
        leaf << decimalStr;
    } else {
        leaf << "";
    }
}

void NMEATenthsUInt16::log(Logger &logger) const {
    if (valuePresent) {
        logger << wholeNumber << "." << tenths;
    } else {
        logger << "NA";
    }
}
