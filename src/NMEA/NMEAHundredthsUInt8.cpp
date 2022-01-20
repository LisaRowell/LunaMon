#include "NMEAHundredthsUInt8.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

#include <Arduino.h>

bool NMEAHundredthsUInt8::set(const String &decimalStr) {
    const unsigned length = decimalStr.length();
    if (length == 0) {
        return false;
    }

    const int periodPos = decimalStr.indexOf(".");
    if (periodPos < 0) {
        if (!extractUInt8FromString(decimalStr, 0, length, wholeNumber)) {
            return false;
        }
        hundredths = 0;
    } else {
        if (periodPos != 0) {
            if (!extractUInt8FromString(decimalStr, 0, periodPos, wholeNumber)) {
                return false;
            }
        } else {
            wholeNumber = 0;
        }

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
    }

    return true;
}

bool NMEAHundredthsUInt8::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
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

void NMEAHundredthsUInt8::publish(DataModelLeaf &leaf) const {
    char decimalStr[8];

    snprintf(decimalStr, 8, "%u.%02u", wholeNumber, hundredths);
    leaf << decimalStr;
}

void NMEAHundredthsUInt8::log(Logger &logger) const {
    char hundredthsStr[4];
    snprintf(hundredthsStr, 4, "%02u", hundredths);
    logger << wholeNumber << "." << hundredthsStr;
}
