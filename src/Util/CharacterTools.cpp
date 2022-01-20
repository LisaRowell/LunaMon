#include "CharacterTools.h"

#include <Arduino.h>

uint8_t decimalValue(char character) {
    return character - '0';
}

bool isUpperCaseHexidecimalDigit(char character) {
    return isDigit(character) || (isUpperCase(character) && isHexadecimalDigit(character));
}

uint8_t hexidecimalValue(char character) {
    if (isDigit(character)) {
        return decimalValue(character);
    } else {
        if (isUpperCase(character)) {
            return character - 'A' + 10;
        } else {
            return character - 'a' + 10;
        }
    }
}

bool isCarriageReturn(char character) {
    return character == '\r';
}

bool isLineFeed(char character) {
    return character == '\n';
}
