#include "NMEALongitude.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"

#include <Arduino.h>

bool NMEALongitude::set(const String &string, const String &eastOrWestStr) {
    if (string.length() < 5) {
        return false;
    }

    if (!setDegrees(string, 0, 3, 180)) {
        return false;
    }

    if (!setMinutes(string, 3)) {
        return false;
    }

    if (eastOrWestStr == "E") {
        eastOrWest = EAST;
    } else if (eastOrWestStr == "W") {
        eastOrWest = WEST;
    } else {
        return false;
    }

    return true;
}

bool NMEALongitude::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String longitudeStr;
    if (!nmeaLine.extractWord(longitudeStr)) {
        logger << logWarning << talker << " " << msgType << " message missing longitude" << eol;
        return false;
    }

    String eastOrWestStr;
    if (!nmeaLine.extractWord(eastOrWestStr)) {
        logger << logWarning << talker << " " << msgType << " message missing E/W" << eol;
        return false;
    }

    if (!set(longitudeStr, eastOrWestStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad longitude '"
               << longitudeStr << "' '" << eastOrWestStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEALongitude::publish(DataModelLeaf &leaf) const {
    switch (eastOrWest) {
        case EAST:
            NMEACoordinate::publish(leaf, true);
            break;

        case WEST:
            NMEACoordinate::publish(leaf, false);
    }
}

void NMEALongitude::log(Logger &logger) const {
    char coordinateStr[20];

    NMEACoordinate::snprint(coordinateStr, 20);
    logger << coordinateStr;

    switch (eastOrWest) {
        case EAST:
            logger << "E";
            break;

        case WEST:
            logger << "W";
    }
}
