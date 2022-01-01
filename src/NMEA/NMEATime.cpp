#include <Arduino.h>

#include "NMEATime.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"
#include "Util/StringTools.h"

bool NMEATime::set(const String &timeStr) {
    if (timeStr.length() < 6) {
        return false;
    }

    if (!extractUInt8FromString(timeStr, 0, 2, hours, 23)) {
        return false;
    }

    if (!extractUInt8FromString(timeStr, 2, 4, minutes, 59)) {
        return false;
    }

    if (!extractUInt8FromString(timeStr, 4, 6, seconds, 59)) {
        return false;
    }

    if (timeStr.length() > 6) {
        if (timeStr.charAt(6) != '.') {
            return false;
        }

        const unsigned startSecondFraction = 7;
        const unsigned endSecondFraction = timeStr.length();
        if (!extractUInt32FromString(timeStr, startSecondFraction, endSecondFraction,
                                     secondFraction)) {
            return false;
        }
        secondPrecision = endSecondFraction - startSecondFraction;
    } else {
        secondFraction = 0;
        secondPrecision = 0;
    }

    return true;
}

bool NMEATime::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String timeStr;
    if (!nmeaLine.extractWord(timeStr)) {
        logger << logWarning << talker << " " << msgType << " message missing Time field" << eol;
        return false;
    }

    if (!set(timeStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad Time field '"
               << timeStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEATime::publish(DataModelLeaf &leaf) const {
    char secondFractionStr[12];
    buildSecondsFactionString(secondFractionStr);

    char timeStr[40];
    snprintf(timeStr, 40, "%02u:%02u:%02u%s", hours, minutes, seconds, secondFractionStr);

    leaf << timeStr;
}

void NMEATime::buildSecondsFactionString(char *string) const {
    if (secondPrecision > 0) {
        *string = '.';
        string++;

        char fractionDigits[11];
        sprintf(fractionDigits, "%lu", secondFraction);

        unsigned leadingZeros = secondPrecision - strlen(fractionDigits);
        while (leadingZeros--) {
            *string = '0';
            string++;
        }

        strcpy(string, fractionDigits);
    } else {
        *string = 0;
    }
}

void NMEATime::log(Logger &logger) const {
    char secondFractionStr[12];
    buildSecondsFactionString(secondFractionStr);

    char timeString[22];
    snprintf(timeString, 22, "%02u:%02u:%02u%s", hours, minutes, seconds, secondFractionStr);

    logger << timeString;
}
