#include "NMEADataValid.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelUInt32Leaf.h"

#include "Util/Logger.h"

#include <etl/string_view.h>

NMEADataValid::NMEADataValid() : valid(false) {
}

bool NMEADataValid::set(etl::string_view &dataValidView) {
    if (dataValidView.size() != 1) {
        return false;
    }

    switch (dataValidView.front()) {
        case 'A':
            valid = true;
            return true;

        case 'V':
            valid = false;
            return true;

        default:
            return false;
    }
}

bool NMEADataValid::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view dataValidView;
    if (!nmeaLine.getWord(dataValidView)) {
        logger << logWarning << talker << " " << msgType << " message missing Data Valid field"
               << eol;
        return false;
    }
    if (!set(dataValidView)) {
        logger << logWarning << talker << " " << msgType << " message with bad Data Valid field '"
               << dataValidView << "'" << eol;
        return false;
    }

    return true;
}

void NMEADataValid::publish(DataModelUInt32Leaf &leaf) const {
    if (valid) {
        leaf = 1;
    } else {
        leaf = 0;
    }
}

void NMEADataValid::log(Logger &logger) const {
    if (valid) {
        logger << "Valid";
    } else {
        logger << "Invalid";
    }
}
