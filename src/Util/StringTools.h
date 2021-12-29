#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

extern bool extractUInt32FromString(const String &string, unsigned start, unsigned end,
                                    uint32_t &value);
extern bool extractUInt8FromString(const String &string, unsigned start, unsigned end,
                                   uint8_t &value, uint8_t maxValue);
extern bool extractUInt32FractionFromDecimalString(const String &string, unsigned decimalStart,
                                                   uint32_t &numerator, uint32_t &denominator);

extern bool isEmptyString(const char *string);

#endif
