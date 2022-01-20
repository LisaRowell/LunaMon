#include "NMEATenthsInt16.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

#include <Arduino.h>

bool NMEATenthsInt16::set(const String &decimalStr) {
    const unsigned length = decimalStr.length();
    if (length == 0) {
        return false;
    }

    const int periodPos = decimalStr.indexOf(".");
    if (periodPos < 0) {
        if (!extractInt16FromString(decimalStr, 0, length, integer)) {
            return false;
        }
        tenths = 0;
    } else {
        if (periodPos != 0) {
            if (!extractInt16FromString(decimalStr, 0, periodPos, integer)) {
                return false;
            }
        } else {
            integer = 0;
        }

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
    }

    return true;
}

bool NMEATenthsInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
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

void NMEATenthsInt16::publish(DataModelLeaf &leaf) const {
    char decimalStr[11];

    snprintf(decimalStr, 11, "%d.%u", integer, tenths);
    leaf << decimalStr;
}

void NMEATenthsInt16::log(Logger &logger) const {
    logger << integer << "." << tenths;
}
