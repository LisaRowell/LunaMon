#include "StringTools.h"
#include "CharacterTools.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>

#include <stdint.h>
#include <stddef.h>

bool extractUInt32FromStringView(const etl::string_view &stringView, size_t start, size_t length,
                                 uint32_t &value, uint32_t maxValue) {
    etl::string_view valueView(stringView.begin() + start, length);
    etl::to_arithmetic_result<uint32_t> conversionResult;
    conversionResult = etl::to_arithmetic<uint32_t>(valueView);
    if (!conversionResult.has_value()) {
        return false;
    }

    if (conversionResult.value() > maxValue) {
        return false;
    }

    value = conversionResult.value();
    return true;
}

bool extractUInt16FromStringView(const etl::string_view &stringView, size_t start, size_t length,
                                 uint16_t &value, uint16_t maxValue) {
    etl::string_view valueView(stringView.begin() + start, length);
    etl::to_arithmetic_result<uint16_t> conversionResult;
    conversionResult = etl::to_arithmetic<uint16_t>(valueView);
    if (!conversionResult.has_value()) {
        return false;
    }

    if (conversionResult.value() > maxValue) {
        return false;
    }

    value = conversionResult.value();
    return true;
}

bool extractUInt8FromStringView(const etl::string_view &stringView, size_t start, size_t length,
                                uint8_t &value, uint8_t maxValue) {
    etl::string_view valueView(stringView.begin() + start, length);
    etl::to_arithmetic_result<uint8_t> conversionResult;
    conversionResult = etl::to_arithmetic<uint8_t>(valueView);
    if (!conversionResult.has_value()) {
        return false;
    }

    if (conversionResult.value() > maxValue) {
        return false;
    }

    value = conversionResult.value();
    return true;
}
