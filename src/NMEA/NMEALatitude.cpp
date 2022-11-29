#include "NMEALatitude.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"

#include <etl/string_view.h>

bool NMEALatitude::set(const etl::string_view &latitudeView,
                       const etl::string_view &northOrSouthView) {
    if (latitudeView.size() < 4) {
        return false;
    }

    etl::string_view degreesView(latitudeView.data(), 2);
    if (!setDegrees(degreesView, 90)) {
        return false;
    }

    etl::string_view minutesView(latitudeView.begin() + 2, latitudeView.end());
    if (!setMinutes(minutesView)) {
        return false;
    }

    if (northOrSouthView.size() != 1) {
        return false;
    }
    switch (northOrSouthView.front()) {
        case 'N':
            northOrSouth = NORTH;
            return true;
        case 'S':
            northOrSouth = SOUTH;
            return true;
        default:
            return false;
    }

    return true;
}

bool NMEALatitude::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view latitudeView;
    if (!nmeaLine.getWord(latitudeView)) {
        logger << logWarning << talker << " " << msgType << " message missing latitude" << eol;
        return false;
    }

    etl::string_view northOrSouthView;
    if (!nmeaLine.getWord(northOrSouthView)) {
        logger << logWarning << talker << " " << msgType << " message missing N/S" << eol;
        return false;
    }

    if (!set(latitudeView, northOrSouthView)) {
        logger << logWarning << talker << " " << msgType << " message with bad latitude '"
               << latitudeView << "' '" << northOrSouthView << "'" << eol;
        return false;
    }

    return true;
}


void NMEALatitude::publish(DataModelStringLeaf &leaf) const {
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
