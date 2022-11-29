#include "NMEAUInt16.h"

#include "Util/Logger.h"
#include "Util/CharacterTools.h"
#include "Util/StringTools.h"
#include "Util/Error.h"

#include <etl/string_view.h>

#include <stdint.h>
#include <stddef.h>

bool NMEAUInt16::set(const etl::string_view &valueView, bool optional, uint16_t maxValue) {
    const size_t length = valueView.size();
    if (length == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    if (!extractUInt16FromStringView(valueView, 0, length, value, maxValue)) {
        valuePresent = false;
        return false;
    }

    valuePresent = true;
    return true;
}

bool NMEAUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                         const char *fieldName, bool optional, uint16_t maxValue) {
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

    if (!set(valueView, optional, maxValue)) {
        logger << logWarning << talker << " " << msgType << " message with bad " << fieldName
               << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

bool NMEAUInt16::hasValue() const {
    return valuePresent;
}

uint16_t NMEAUInt16::getValue() const {
    if (!valuePresent) {
        fatalError("Attempt to read value from NMEAUInt8 with value not present");
    }

    return value;
}

void NMEAUInt16::publish(DataModelUInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}

void NMEAUInt16::log(Logger &logger) const {
    if (valuePresent) {
        logger << value;
    } else {
        logger << "NA";
    }
}
