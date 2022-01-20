#ifndef CHARACTER_TOOLS_H
#define CHARACTER_TOOLS_H

#include <Arduino.h>

extern uint8_t decimalValue(char character);
extern bool isUpperCaseHexidecimalDigit(char character);
extern uint8_t hexidecimalValue(char character);
extern bool isCarriageReturn(char character);
extern bool isLineFeed(char character);

#endif
