#include <Arduino.h>

#include "NMEAHundredthsUInt16.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

bool NMEAHundredthsUInt16::set(const String &decimalStr) {
    const unsigned length = decimalStr.length();
    if (length == 0) {
        return false;
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
            const unsigned placesToRead = min((unsigned)3, charactersAfterPeriod);
            const unsigned decimalEnd = decimalStart + placesToRead;
            uint16_t decimalPortion;
            if (!extractUInt16FromString(decimalStr, decimalStart, decimalEnd, decimalPortion)) {
                return false;
            }
            if (placesToRead > 2) {
                hundredths = (decimalPortion + 5) / 10;
            } else {
                hundredths = decimalPortion;
            }
        } else {
            hundredths = 0;
        }
    } else {
        hundredths = 0;
    }

    return true;
}

bool NMEAHundredthsUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                                   const char *fieldName) {
    String decimalStr;
    if (!nmeaLine.extractWord(decimalStr)) {
        logger << logWarning << talker << " " << msgType << " message missing " << fieldName
               << " field" << eol;
        return false;
    }

    if (!set(decimalStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << decimalStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEAHundredthsUInt16::publish(DataModelLeaf &leaf) const {
    char decimalStr[11];

    snprintf(decimalStr, 11, "%u.%02u", wholeNumber, hundredths);
    leaf << decimalStr;
}

void NMEAHundredthsUInt16::log(Logger &logger) const {
    char hundredthsStr[4];
    snprintf(hundredthsStr, 4, "%02u", hundredths);
    logger << wholeNumber << "." << hundredthsStr;
}
