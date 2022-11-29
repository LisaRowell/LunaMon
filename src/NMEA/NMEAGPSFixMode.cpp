#include "NMEAGPSFixMode.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

bool NMEAGPSFixMode::set(etl::string_view &gpsFixModeView) {
    if (gpsFixModeView.size() != 1) {
        return false;
    }

    const char gpsFixModeChar = gpsFixModeView.front();
    if (!isDigit(gpsFixModeChar)) {
        return false;
    }
    if (decimalValue(gpsFixModeChar) > GPS_FIX_MODE_3D) {
        return false;
    }
    gpsFixMode = (GPSFixMode)decimalValue(gpsFixModeChar);

    return true;
}

bool NMEAGPSFixMode::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view gpsFixModeView;
    if (!nmeaLine.getWord(gpsFixModeView)) {
        logger << logWarning << talker << " " << msgType << " message missing GPS Fix Mode field"
               << eol;
        return false;
    }

    if (!set(gpsFixModeView)) {
        logger << logWarning << talker << " " << msgType << " message with bad GPS Fix Mode field '"
               << gpsFixModeView << "'" << eol;
        return false;
    }

    return true;
}

void NMEAGPSFixMode::publish(DataModelStringLeaf &leaf) const {
    switch (gpsFixMode) {
        case GPS_FIX_MODE_NO_FIX:
            leaf = "None";
            break;

        case GPS_FIX_MODE_2D:
            leaf = "2D";
            break;

        case GPS_FIX_MODE_3D:
            leaf = "3D";
    }
}

void NMEAGPSFixMode::log(Logger &logger) const {
    switch (gpsFixMode) {
        case GPS_FIX_MODE_NO_FIX:
            logger << "No Fix";
            break;

        case GPS_FIX_MODE_2D:
            logger << "2D Fix";
            break;

        case GPS_FIX_MODE_3D:
            logger << "3D Fix";
    }
}
