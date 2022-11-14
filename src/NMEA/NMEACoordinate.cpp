#include "NMEACoordinate.h"

#include "DataModel/DataModel.h"
#include "DataModel/DataModelStringLeaf.h"

#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <Arduino.h>

using etl::string;
using etl::string_stream;

bool NMEACoordinate::setDegrees(const String &string, unsigned startDegrees, unsigned endDegrees,
                                uint8_t maxDegrees) {
    if (!extractUInt8FromString(string, startDegrees, endDegrees, degrees, maxDegrees)) {
        return false;
    }

    return true;
}

bool NMEACoordinate::setMinutes(const String &string, unsigned startMinutes) {
    uint8_t wholeMinutes;
    const unsigned endWholeMinutes = startMinutes + 2;
    if (!extractUInt8FromString(string, startMinutes, endWholeMinutes, wholeMinutes, 59)) {
        return false;
    }

    if (string.length() > endWholeMinutes) {
        if (string.charAt(endWholeMinutes) != '.') {
            return false;
        }

        const unsigned startMinuteFraction = endWholeMinutes + 1;
        const unsigned endMinuteFraction = string.length();

        uint32_t numerator = 0;
        uint32_t denominator = 1;

        unsigned fractionPos;
        for (fractionPos = startMinuteFraction; fractionPos < endMinuteFraction; fractionPos++) {
            char digit = string.charAt(fractionPos);
            if (!isDigit(digit)) {
                return false;
            }
            numerator = numerator * 10 + decimalValue(digit);
            denominator = denominator * 10;
        }

        minutes = (float)wholeMinutes + (float)numerator / denominator;
    } else {
        minutes = (float)wholeMinutes;
    }

    return true;
}

// This prints the coordinate as unsigned and the caller is responsible for appending N/S or E/W.
void NMEACoordinate::snprint(char *string, size_t maxLength) const {
    snprintf(string, maxLength, "%u\xC2\xB0%05f'", degrees, minutes);
}

// We publish coordinates as a string containing a signed, floating point number of degrees.
// Clients are responsible for displaying the values in a way that matches the users preference.
void NMEACoordinate::publish(DataModelStringLeaf &leaf, bool isPositive) const {
    string<coordinateLength> coordinateStr;
    string_stream coordinateStream(coordinateStr);

    if (!isPositive) {
        coordinateStream << "-";
    }
    float coordinateFloat = degrees + minutes / 60;
    coordinateStream << etl::setprecision(4) << coordinateFloat;

    leaf = coordinateStr;
}
