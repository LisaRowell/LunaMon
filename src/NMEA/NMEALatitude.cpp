#include "NMEALatitude.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"

#include <Arduino.h>

bool NMEALatitude::set(const String &string, const String &northOrSouthStr) {
    if (string.length() < 4) {
        return false;
    }

    if (!setDegrees(string, 0, 2, 90)) {
        return false;
    }

    if (!setMinutes(string, 2)) {
        return false;
    }

    if (northOrSouthStr == "N") {
        northOrSouth = NORTH;
    } else if (northOrSouthStr == "S") {
        northOrSouth = SOUTH;
    } else {
        return false;
    }

    return true;
}

bool NMEALatitude::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String latitudeStr;
    if (!nmeaLine.extractWord(latitudeStr)) {
        logger << logWarning << talker << " " << msgType << " message missing latitude" << eol;
        return false;
    }

    String northOrSouthStr;
    if (!nmeaLine.extractWord(northOrSouthStr)) {
        logger << logWarning << talker << " " << msgType << " message missing N/S" << eol;
        return false;
    }

    if (!set(latitudeStr, northOrSouthStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad latitude '"
               << latitudeStr << "' '" << northOrSouthStr << "'" << eol;
        return false;
    }

    return true;
}


void NMEALatitude::publish(DataModelLeaf &leaf) const {
    switch (northOrSouth) {
        case NORTH:
            NMEACoordinate::publish(leaf, true);
            break;

        case SOUTH:
            NMEACoordinate::publish(leaf, false);
    }
}

void NMEALatitude::log(Logger &logger) const {
    char coordinateStr[20];

    NMEACoordinate::snprint(coordinateStr, 20);
    logger << coordinateStr;

    switch (northOrSouth) {
        case NORTH:
            logger << "N";
            break;

        case SOUTH:
            logger << "S";
    }
}
