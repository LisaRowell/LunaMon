#include <Arduino.h>

#include "NMEAGLLMessage.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEAFAAModeIndicator.h"

#include "DataModel/DataModel.h"

#include "Util/Logger.h"

void parseNMEAGLLMessage(NMEATalker talker, NMEALine &nmeaLine) {
    String latitudeStr;
    if (!nmeaLine.extractWord(latitudeStr)) {
        logger << logWarning << talker << " GLL message missing latitude" << eol;
        return;
    }

    String northOrSouthStr;
    if (!nmeaLine.extractWord(northOrSouthStr)) {
        logger << logWarning << talker << " GLL message missing N/S" << eol;
        return;
    }

    NMEALatitude latitude;
    if (!latitude.set(latitudeStr, northOrSouthStr)) {
        logger << logWarning << talker << " GLL message with bad latitude '" << latitudeStr << "' '"
               << northOrSouthStr << "'" << eol;
        return;
    }

    String longitudeStr;
    if (!nmeaLine.extractWord(longitudeStr)) {
        logger << logWarning << talker << " GLL message missing longitude" << eol;
        return;
    }

    String eastOrWestStr;
    if (!nmeaLine.extractWord(eastOrWestStr)) {
        logger << logWarning << talker << " GLL message missing E/W" << eol;
        return;
    }

    NMEALongitude longitude;
    if (!longitude.set(longitudeStr, eastOrWestStr)) {
        logger << logWarning << talker << " GLL message with bad longitude '" << longitudeStr
               << "' '" << eastOrWestStr << "'" << eol;
        return;
    }

    Time time;
    String timeStr;
    if (!nmeaLine.extractWord(timeStr)) {
        logger << logWarning << talker << " GLL message missing Time field" << eol;
        return;
    }
    if (!time.set(timeStr)) {
        logger << logWarning << talker << " GLL message with bad Time field '" << timeStr << "'"
               << eol;
        return;
    }

    NMEADataValid dataValid;
    String dataValidStr;
    if (!nmeaLine.extractWord(dataValidStr)) {
        logger << logWarning << talker << " GLL message missing Data Valid field" << eol;
        return;
    }
    if (!dataValid.set(dataValidStr)) {
        logger << logWarning << talker << " GLL message with bad Data Valid field '" << dataValidStr
               << "'" << eol;
        return;
    }

    NMEAFAAModeIndicator faaModeIndicator;
    String faaModeIndicatorStr;
    if (nmeaLine.extractWord(faaModeIndicatorStr)) {
        if (!faaModeIndicator.set(faaModeIndicatorStr)) {
            logger << logWarning << talker << " GLL message with bad FAA Mode Indicator field '"
                   << faaModeIndicatorStr << "'" << eol;
            return;
        }
    }

    latitude.publish(positionLatitude);
    longitude.publish(positionLongitude);
    time.publish(positionTime);
    dataValid.publish(positionDataValid);
    faaModeIndicator.publish(positionFAAModeindicator);

    Serial.print(nmeaTalkerName(talker));
    Serial.print(" GLL: ");
    latitude.print();
    Serial.print(" ");
    longitude.print();
    Serial.print(" ");
    time.print();
    Serial.print(" ");
    dataValid.print();

    if (faaModeIndicator.hasValue()) {
        Serial.print(" ");
        faaModeIndicator.print();
    }

    Serial.println();
}