#include "NMEAGPSFixMode.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"

#include <etl/string.h>

#include <Arduino.h>

using etl::string;

bool NMEAGPSFixMode::set(String &gpsFixModeStr) {
    if (gpsFixModeStr.length() != 1) {
        return false;
    }

    const char gpsFixModeChar = gpsFixModeStr.charAt(0);
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
    String gpsFixModeStr;
    if (!nmeaLine.extractWord(gpsFixModeStr)) {
        logger << logWarning << talker << " " << msgType << " message missing GPS Fix Mode field"
               << eol;
        return false;
    }

    if (!set(gpsFixModeStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad GPS Fix Mode field '"
               << gpsFixModeStr << "'" << eol;
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
