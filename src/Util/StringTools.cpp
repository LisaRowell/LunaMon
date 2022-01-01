#include <Arduino.h>

#include "StringTools.h"
#include "CharacterTools.h"

bool extractUInt32FromString(const String &string, unsigned start, unsigned end, uint32_t &value) {
    value = 0;

    unsigned pos;
    for (pos = start; pos < end; pos++) {
        char valueChar = string.charAt(pos);
        if (!isDigit(valueChar)) {
            return false;
        }

        value = value * 10 + decimalValue(valueChar);
    }

    return true;
}

bool extractUInt16FromString(const String &string, unsigned start, unsigned end, uint16_t &value,
                             uint16_t maxValue) {
    uint32_t uint32Value = 0;
    if (!extractUInt32FromString(string, start, end, uint32Value)) {
        return false;
    }

    if (uint32Value > maxValue) {
        return false;
    }

    value = (uint16_t)uint32Value;

    return true;
}

bool extractUInt8FromString(const String &string, unsigned start, unsigned end, uint8_t &value,
                            uint8_t maxValue) {
    uint32_t uint32Value = 0;
    if (!extractUInt32FromString(string, start, end, uint32Value)) {
        return false;
    }

    if (uint32Value > maxValue) {
        return false;
    }

    value = (uint8_t)uint32Value;    

    return true;
}

bool extractDigitFromString(const String &string, unsigned pos, uint8_t &value) {
    const char digitChar = string.charAt(pos);

    if (!isDigit(digitChar)) {
        return false;
    }

    value = decimalValue(digitChar);

    return true;
}

bool extractUInt32FractionFromDecimalString(const String &string, unsigned decimalStart,
                                            uint32_t &numerator, uint32_t &denominator) {
    if (string.charAt(decimalStart) != '.') {
        return false;
    }

    numerator = 0;
    denominator = 1;

    unsigned stringPos;
    unsigned stringLength = string.length();
    for (stringPos = decimalStart; stringPos < stringLength; stringPos++) {
        char decimalChar = string.charAt(stringPos);
        if (!isDigit(decimalChar)) {
            return false;
        }

        numerator = numerator * 10 + decimalValue(decimalChar);
        denominator = denominator * 10;
    }

    return true;
}

bool convertTwoDigitDecimalString(const String &string, uint8_t &value) {
    if (string.length() != 2) {
        return false;
    }

    value = decimalValue(string.charAt(0)) * 10 + decimalValue(string.charAt(1));

    return true;
}

bool isEmptyString(const char *string) {
    return *string == 0;
}
