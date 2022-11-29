#include "NMEATenthsUInt16.h"

#include "Util/Logger.h"
#include "Util/Error.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>
#include <etl/string.h>
#include <etl/string_stream.h>

bool NMEATenthsUInt16::set(const etl::string_view &valueView, bool optional) {
    if (valueView.size() == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::string_view wholeNumberView;
    size_t periodPos = valueView.find('.');
    if (periodPos == valueView.npos) {
        wholeNumberView = etl::string_view(valueView);
    } else {
        wholeNumberView = etl::string_view(valueView.begin(), periodPos);
    }
    etl::to_arithmetic_result<uint16_t> wholeNumberResult;
    wholeNumberResult = etl::to_arithmetic<uint16_t>(wholeNumberView);
    if (!wholeNumberResult.has_value()) {
        valuePresent = false;
        return false;
    }
    wholeNumber = wholeNumberResult.value();

    if (periodPos != valueView.npos && valueView.length() > periodPos + 1) {
        etl::string_view decimalView(valueView.begin() + periodPos, valueView.end());
        decimalView.remove_prefix(1);
        if (decimalView.length() > 2) {
            decimalView.remove_suffix(decimalView.length() - 2);
        }
        etl::to_arithmetic_result<uint8_t> decimalResult;
        decimalResult = etl::to_arithmetic<uint8_t>(decimalView);
        if (!decimalResult.has_value()) {
            valuePresent = false;
            return false;
        }
        switch (decimalView.length()) {
            case 1:
                tenths = decimalResult.value();
                break;
            case 2:
                tenths = (decimalResult.value() + 5) / 10;
                break;
            default:
                fatalError("Bad parsing of TenthsUint16");
        }
    } else {
        tenths = 0;
    }

    valuePresent = true;
    return true;
}

bool NMEATenthsUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                               const char *fieldName, bool optional) {
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

    if (!set(valueView, optional)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

bool NMEATenthsUInt16::hasValue() const {
    return valuePresent;
}

void NMEATenthsUInt16::publish(DataModelTenthsUInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf.set(wholeNumber, tenths);
    } else {
        leaf.removeValue();
    }
}

void NMEATenthsUInt16::log(Logger &logger) const {
    if (valuePresent) {
        logger << wholeNumber << "." << tenths;
    } else {
        logger << "NA";
    }
}
