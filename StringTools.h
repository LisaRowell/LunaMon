#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

extern bool extractUInt32FromString(const String &string, unsigned start, unsigned end, uint32_t &value);
extern bool extractUInt8FromString(const String &string, unsigned start, unsigned end, uint8_t &value,
                                   uint8_t maxValue);
extern bool extractUInt32FractionFromDecimalString(const String &string, unsigned decimalStart,
                                                   uint32_t &numerator, uint32_t &denominator);

#endif