#include <Arduino.h>

#include "NMEATalker.h"
#include "Util/Error.h"

enum NMEATalker parseNMEATalker(String &talkerCode) {
    if (talkerCode == "GP") {
        return NMEA_TALKER_GPS;
    } else {
        return NMEA_TALKER_UNKNOWN;
    }
}

const char *nmeaTalkerName(enum NMEATalker talker) {
    switch (talker) {
        case NMEA_TALKER_GPS:
        return "GPS";

        case NMEA_TALKER_UNKNOWN:
        return "Unknown";

        default:
        fatalError("Messed up NMEATalker enumeration found while getting name.");
    }
}
