#include "NMEAMagneticVariation.h"

#include "Util/CharacterTools.h"
#include "Util/StringTools.h"
#include "Util/Logger.h"

#include <Arduino.h>

bool NMEAMagneticVariation::set(const String &directionStr, const String &eastOrWestStr) {
    const unsigned directionLength = directionStr.length();
    if (directionLength == 0 && eastOrWestStr.length() == 0) {
        hasValue = false;
        return true;
    }
    if (directionLength == 0) {
        return false;
    }

    uint16_t directionAbsoluteValue;
    int periodPos = directionStr.indexOf(".");
    if (periodPos > 0) {
        if (!extractUInt16FromString(directionStr, 0, periodPos, directionAbsoluteValue, 180)) {
            return false;
        }

        if (directionLength > (unsigned)periodPos + 1) {
            if (!extractDigitFromString(directionStr, periodPos + 1, tenths)) {
                return false;
            }
        }
    } else if (periodPos == 0) {
        directionAbsoluteValue = 0;
        if (directionLength > 1) {
            if (!extractDigitFromString(directionStr, 1, tenths)) {
                return false;
            }
        }
    } else {
        if (!extractUInt16FromString(directionStr, 0, directionLength, directionAbsoluteValue)) {
            return false;
        }
        tenths = 0;
    }
    
    if (eastOrWestStr == "W") {
        direction = directionAbsoluteValue;
    } else if (eastOrWestStr == "E") {
        direction = -directionAbsoluteValue;
    } else {
        return false;
    }

    hasValue = true;
    return true;
}

bool NMEAMagneticVariation::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String directionStr;
    if (!nmeaLine.extractWord(directionStr)) {
        logger << logWarning << talker << " " << msgType
               << " message missing Magnetic Variation direction field" << eol;
        return false;
    }
    String eastOrWestStr;
    if (!nmeaLine.extractWord(eastOrWestStr)) {
        logger << logWarning << talker << " " << msgType
               << " message missing Magnetic Variation E/W field" << eol;
        return false;
    }

    if (!set(directionStr, eastOrWestStr)) {
        logger << logWarning << talker << " " << msgType
               << " message with bad Magnetic Variation field '" << directionStr << ","
               << eastOrWestStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEAMagneticVariation::publish(DataModelTenthsInt16Leaf &leaf) const {
    if (hasValue) {
        leaf.set(direction, tenths);
    } else {
        leaf.removeValue();
    }
}

void NMEAMagneticVariation::log(Logger &logger) const {
    if (hasValue) {
        logger << direction << "." << tenths << "\xC2\xB0";
    } else {
        logger << "Unknown\xC2\xB0";
    }
}
