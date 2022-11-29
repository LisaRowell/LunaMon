#include "NMEALongitude.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"

#include <etl/string_view.h>

bool NMEALongitude::set(const etl::string_view &longitudeView,
                        const etl::string_view &eastOrWestView) {
    if (longitudeView.size() < 5) {
        return false;
    }

    etl::string_view degreesView(longitudeView.data(), 3);
    if (!setDegrees(degreesView, 180)) {
        return false;
    }

    etl::string_view minutesView(longitudeView.begin() + 3, longitudeView.end());
    if (!setMinutes(minutesView)) {
        return false;
    }

    if (eastOrWestView.size() != 1) {
        return false;
    }
    switch (eastOrWestView.front()) {
        case 'E':
            eastOrWest = EAST;
            return true;
        case 'W':
            eastOrWest = WEST;
            return true;
        default:
            return false;
    }
}

bool NMEALongitude::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view longitudeView;
    if (!nmeaLine.getWord(longitudeView)) {
        logger << logWarning << talker << " " << msgType << " message missing longitude" << eol;
        return false;
    }

    etl::string_view eastOrWestView;
    if (!nmeaLine.getWord(eastOrWestView)) {
        logger << logWarning << talker << " " << msgType << " message missing E/W" << eol;
        return false;
    }

    if (!set(longitudeView, eastOrWestView)) {
        logger << logWarning << talker << " " << msgType << " message with bad longitude '"
               << longitudeView << "' '" << eastOrWestView << "'" << eol;
        return false;
    }

    return true;
}

void NMEALongitude::publish(DataModelStringLeaf &leaf) const {
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
