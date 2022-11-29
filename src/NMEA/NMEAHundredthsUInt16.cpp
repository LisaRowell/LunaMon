#include "NMEAHundredthsUInt16.h"

#include "Util/Logger.h"
#include "Util/Error.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>
#include <etl/string.h>
#include <etl/string_stream.h>

bool NMEAHundredthsUInt16::set(const etl::string_view &valueView) {
    if (valueView.size() == 0) {
        return false;
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
        return false;
    }
    wholeNumber = wholeNumberResult.value();

    if (periodPos != valueView.npos && valueView.length() > periodPos + 1) {
        etl::string_view decimalView(valueView.begin() + periodPos, valueView.end());
        decimalView.remove_prefix(1);
        if (decimalView.length() > 3) {
            decimalView.remove_suffix(decimalView.length() - 3);
        }
        etl::to_arithmetic_result<uint16_t> decimalResult;
        decimalResult = etl::to_arithmetic<uint16_t>(decimalView);
        if (!decimalResult.has_value()) {
            return false;
        }
        switch (decimalView.length()) {
            case 1:
                hundredths = decimalResult.value() * 10;
                break;
            case 2:
                hundredths = decimalResult.value();
                break;
            case 3:
                hundredths = (decimalResult.value() + 5) / 10;
                break;
            default:
                fatalError("Bad parsing of HundredthsUint16");
        }
    } else {
        hundredths = 0;
    }

    return true;
}

bool NMEAHundredthsUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                                   const char *fieldName) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        logger << logWarning << talker << " " << msgType << " message missing " << fieldName
               << " field" << eol;
        return false;
    }

    if (!set(valueView)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

void NMEAHundredthsUInt16::publish(DataModelHundredthsUInt16Leaf &leaf) const {
    leaf.set(wholeNumber, hundredths);
}

void NMEAHundredthsUInt16::log(Logger &logger) const {
    etl::string<8> decimalStr;
    etl::string_stream decimalStream(decimalStr);
    decimalStream << wholeNumber << "." << etl::setw(2) << etl::setfill('0') << hundredths;
    logger << decimalStr;
}
