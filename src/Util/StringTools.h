#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <etl/string_view.h>

#include <stdint.h>
#include <stddef.h>

extern bool extractUInt32FromStringView(const etl::string_view &stringView, size_t start,
                                        size_t length, uint32_t &value,
                                        uint32_t maxValue = 0xffffffff);
extern bool extractUInt16FromStringView(const etl::string_view &stringView, size_t start,
                                        size_t length, uint16_t &value, uint16_t maxValue = 0xffff);
extern bool extractUInt8FromStringView(const etl::string_view &stringView, size_t start,
                                       size_t length, uint8_t &value, uint8_t maxValue = 0xff);

#endif
