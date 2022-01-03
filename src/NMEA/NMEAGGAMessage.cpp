#include <Arduino.h>

#include "NMEAGGAMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEAHundredthsUInt16.h"
#include "NMEATenthsInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/StringTools.h"
#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAGGAMessage::NMEAGGAMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGGAMessage::parse(NMEALine &nmeaLine) {
    if (!time.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!latitude.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!longitude.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!gpsQuality.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!numberSatellites.extract(nmeaLine, talker, "GGA", "Number Satellites", false, 12)) {
        return false;
    }

    if (!horizontalDilutionOfPrecision.extract(nmeaLine, talker, "GGA",
                                               "Horizontal Dilution of Precision")) {
        return false;
    }

    if (!antennaAltitude.extract(nmeaLine, talker, "GGA", "Antenna Altitude")) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "GGA", "M")) {
        return false;
    }

    if (!geoidalSeparation.extract(nmeaLine, talker, "GGA", "Geoidal Separation")) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "GGA", "M")) {
        return false;
    }

    if (!gpsDataAge.extract(nmeaLine, talker, "GGA", "GPS Data Age", true)) {
        return false;
    }

    if (!differentialReferenceStation.extract(nmeaLine, talker, "GGA",
                                              "Differential Reference Station", true, 1023)) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAGGAMessage::type() {
    return NMEA_MSG_TYPE_GGA;
}

void NMEAGGAMessage::log() {
    logger << logDebug << talker << " GGA: " << time << " " << latitude << " " << longitude << " "
           << gpsQuality << " " << numberSatellites << " " << horizontalDilutionOfPrecision << " "
           << antennaAltitude << "m " << geoidalSeparation << "m";
    
    if (gpsDataAge.hasValue()) {
        logger << " " << gpsDataAge;
    }

    if (differentialReferenceStation.hasValue()) {
        logger << " " << differentialReferenceStation;
    }

    logger << eol;
}

NMEAGGAMessage *parseNMEAGGAMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGGAMessage *message = new (nmeaMessageBuffer)NMEAGGAMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages,
        // we don't do a free here.
        return NULL;
    }

    return message;
}
