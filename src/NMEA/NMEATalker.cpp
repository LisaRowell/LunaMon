#include <Arduino.h>

#include "NMEATalker.h"

#include "Util/Logger.h"
#include "Util/Error.h"

NMEATalker::NMEATalker (String &talkerCode) {
    if (talkerCode == "GP") {
        talker = NMEA_TALKER_GPS;
    } else {
        talker = NMEA_TALKER_UNKNOWN;
    }
}

const char *NMEATalker::name() const {
    switch (talker) {
        case NMEA_TALKER_GPS:
        return "GPS";

        case NMEA_TALKER_UNKNOWN:
        return "Unknown";

        default:
        fatalError("Messed up NMEATalker enumeration found while getting name.");
    }
}

void NMEATalker::log(Logger &logger) const {
    logger << name();
}
