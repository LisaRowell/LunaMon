#include "NMEAGPSQuality.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"

#include <Arduino.h>

bool NMEAGPSQuality::set(String &gpsQualityStr) {
    if (gpsQualityStr.length() != 1) {
        return false;
    }

    const char gpsQualityChar = gpsQualityStr.charAt(0);
    if (!isDigit(gpsQualityChar)) {
        return false;
    }
    if (decimalValue(gpsQualityChar) > GPS_QUALITY_SIMULATED_MODE) {
        return false;
    }
    gpsQuality = (GPSQuality)decimalValue(gpsQualityChar);

    return true;
}

bool NMEAGPSQuality::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String gpsQualityStr;
    if (!nmeaLine.extractWord(gpsQualityStr)) {
        logger << logWarning << talker << " " << msgType << " message missing GPS Quality field"
               << eol;
        return false;
    }

    if (!set(gpsQualityStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad GPS Quality field '"
               << gpsQualityStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEAGPSQuality::publish(DataModelStringLeaf &leaf) const {
    switch (gpsQuality) {
        case GPS_QUALITY_FIX_NOT_AVAILABLE:
            leaf = "Fix Not Available";
            break;

        case GPS_QUALITY_GPS_FIX:
            leaf = "GPS Fix";
            break;

        case GPS_QUALITY_DIFFERENTIAL_GPS_FIX:
            leaf = "Differential GPS Fix";
            break;

        case GPS_QUALITY_PPS_FIX:
            leaf = "PPS Fix";
            break;

        case GPS_QUALITY_REAL_TIME_KINEMATIC:
            leaf = "Real Time Kinematic";
            break;

        case GPS_QUALITY_FLOAT_RTK:
            leaf = "Float RTK";
            break;

        case GPS_QUALITY_ESTIMATED:
            leaf = "Estimated";
            break;

        case GPS_QUALITY_MANUAL_INPUT_MODE:
            leaf = "Manual Input Mode";
            break;

        case GPS_QUALITY_SIMULATED_MODE:
            leaf = "Simulated Mode";
    }
}

void NMEAGPSQuality::log(Logger &logger) const {
    switch (gpsQuality) {
        case GPS_QUALITY_FIX_NOT_AVAILABLE:
            logger << "Fix Not Available";
            break;

        case GPS_QUALITY_GPS_FIX:
            logger << "GPS Fix";
            break;

        case GPS_QUALITY_DIFFERENTIAL_GPS_FIX:
            logger << "Differential GPS Fix";
            break;

        case GPS_QUALITY_PPS_FIX:
            logger << "PPS Fix";
            break;

        case GPS_QUALITY_REAL_TIME_KINEMATIC:
            logger << "Real Time Kinematic";
            break;

        case GPS_QUALITY_FLOAT_RTK:
            logger << "Float RTK";
            break;

        case GPS_QUALITY_ESTIMATED:
            logger << "Estimated";
            break;

        case GPS_QUALITY_MANUAL_INPUT_MODE:
            logger << "Manual Input Mode";
            break;

        case GPS_QUALITY_SIMULATED_MODE:
            logger << "Simulated Mode";
    }
}
