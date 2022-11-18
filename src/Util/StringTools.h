#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <Arduino.h>

extern bool extractUInt32FromString(const String &string, unsigned start, unsigned end,
                                    uint32_t &value);
extern bool extractUInt16FromString(const String &string, unsigned start, unsigned end,
                                    uint16_t &value, uint16_t maxValue = 0xffff);
extern bool extractUInt8FromString(const String &string, unsigned start, unsigned end,
                                   uint8_t &value, uint8_t maxValue = 0xff);
extern bool extractDigitFromString(const String &string, unsigned pos, uint8_t &value);
extern bool extractUInt32FractionFromDecimalString(const String &string, unsigned decimalStart,
                                                   uint32_t &numerator, uint32_t &denominator);
extern bool extractInt16FromString(const String &string, unsigned start, unsigned end,
                                   int16_t &value);
extern bool extractInt8FromString(const String &string, unsigned start, unsigned end,
                                  int8_t &value, int8_t minValue = -128, int8_t maxValue = 127);
extern bool convertTwoDigitDecimalString(const String &string, uint8_t &value,
                                         uint8_t maxValue = 99);

#endif
