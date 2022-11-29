#include "NMEAInt8.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>

#include <stddef.h>
#include <stdint.h>

bool NMEAInt8::set(const etl::string_view &valueView, bool optional, int8_t minValue,
                   int8_t maxValue) {
    const size_t length = valueView.size();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::to_arithmetic_result<int8_t> conversionResult = etl::to_arithmetic<int8_t>(valueView);
    if (!conversionResult.has_value()) {
        valuePresent = false;
        return false;
    }

    if (conversionResult.value() > maxValue || conversionResult.value() < minValue) {
        valuePresent = false;
        return false;
    }

    value = conversionResult.value();
    valuePresent = true;

    return true;
}

bool NMEAInt8::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                       const char *fieldName, bool optional, int8_t minValue, int8_t maxValue) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        if (!optional) {
            logger << logWarning << talker << " " << msgType << " message missing " << fieldName
                   << " field" << eol;
            return false;
        }

        valuePresent = false;
        return true;
    }

    if (!set(valueView, optional, minValue, maxValue)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

void NMEAInt8::publish(DataModelInt8Leaf &leaf) const {
    leaf = value;
}

void NMEAInt8::log(Logger &logger) const {
    logger << value;
}
