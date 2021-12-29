#include <Arduino.h>

#include "NMEATime.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/StringTools.h"

bool Time::set(const String &timeStr) {
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

void Time::print() {
    Serial.print(hours / 10);
    Serial.print(hours % 10);
    Serial.print(":");
    Serial.print(minutes / 10);
    Serial.print(minutes % 10);
    Serial.print(":");
    Serial.print(seconds / 10);
    Serial.print(seconds % 10);

    char secondFractionStr[12];
    buildSecondsFactionString(secondFractionStr);
    Serial.print(secondFractionStr);
}

void Time::publish(DataModelLeaf &leaf) {
    char secondFractionStr[12];
    buildSecondsFactionString(secondFractionStr);

    char timeStr[40];
    snprintf(timeStr, 40, "%02u:%02u:%02u%s", hours, minutes, seconds, secondFractionStr);

    leaf << timeStr;
}

void Time::buildSecondsFactionString(char *string) {
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
