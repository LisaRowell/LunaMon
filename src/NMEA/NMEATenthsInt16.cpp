#include "NMEATenthsInt16.h"

#include "Util/Logger.h"
#include "Util/Error.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>

bool NMEATenthsInt16::set(const etl::string_view &valueView, bool optional) {
    if (valueView.size() == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::string_view integerView;
    size_t periodPos = valueView.find('.');
    if (periodPos == valueView.npos) {
        integerView = etl::string_view(valueView);
    } else {
        integerView = etl::string_view(valueView.begin(), periodPos);
    }
    etl::to_arithmetic_result<int16_t> integerResult = etl::to_arithmetic<int16_t>(integerView);
    if (!integerResult.has_value()) {
        valuePresent = false;
        return false;
    }
    integer = integerResult.value();

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
                fatalError("Bad parsing of TenthsInt16");
        }
    } else {
        tenths = 0;
    }

    valuePresent = true;
    return true;
}

bool NMEATenthsInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                              const char *fieldName, bool optional) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        if (!optional) {
            logger << logWarning << talker << " " << msgType << " message missing " << fieldName
                   << " field" << eol;
            valuePresent = false;
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

void NMEATenthsInt16::publish(DataModelTenthsInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf.set(integer, tenths);
    } else {
        leaf.removeValue();
    }
}

void NMEATenthsInt16::log(Logger &logger) const {
    if (valuePresent) {
        logger << integer << "." << tenths;
    } else {
        logger << "NA";
    }
}
