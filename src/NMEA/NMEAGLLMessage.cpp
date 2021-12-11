#include <Arduino.h>

#include "NMEAGLLMessage.h"
#include "NMEAMessageBuffer.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Time.h"
#include "NMEADataValid.h"
#include "NMEAFAAModeIndicator.h"
#include "Util/PlacementNew.h"
#include "Util/Logger.h"

bool NMEAGLLMessage::parse(NMEATalker talker, NMEALine &nmeaLine) {
    String latitudeStr;
    if (!nmeaLine.extractWord(latitudeStr)) {
        logger << logWarning << talker << " GLL message missing latitude" << eol;
        return false;
    }

    String northOrSouthStr;
    if (!nmeaLine.extractWord(northOrSouthStr)) {
        logger << logWarning << talker << " GLL message missing N/S" << eol;
        return false;
    }

    if (!latitude.set(latitudeStr, northOrSouthStr)) {
        logger << logWarning << talker << " GLL message with bad latitude" << eol;
        return false;
    }

    String longitudeStr;
    if (!nmeaLine.extractWord(longitudeStr)) {
        logger << logWarning << talker << " GLL message missing longitude" << eol;
        return false;
    }

    String eastOrWestStr;
    if (!nmeaLine.extractWord(eastOrWestStr)) {
        logger << logWarning << talker << " GLL message missing E/W" << eol;
        return false;
    }

    if (!longitude.set(longitudeStr, eastOrWestStr)) {
        logger << logWarning << talker << " GLL message with bad longitude" << eol;
        return false;
    }

    String timeStr;
    if (!nmeaLine.extractWord(timeStr)) {
        logger << logWarning << talker << " GLL message missing time" << eol;
        return false;
    }
    if (!time.set(timeStr)) {
        logger << logWarning << talker << " GLL message with bad time" << eol;
        return false;
    }

    String dataValidStr;
    if (!nmeaLine.extractWord(dataValidStr)) {
        logger << logWarning << talker << " GLL message missing data valid field" << eol;
        return false;
    }
    if (!parseNMEADataValid(dataValidStr, dataValid)) {
        logger << logWarning << talker << " GLL message with bad data valid field" << eol;
        return false;
    }

    String faaModeIndicatorStr;
    if (nmeaLine.extractWord(faaModeIndicatorStr)) {
        if (!parseNMEAFAAModeIndicator(faaModeIndicatorStr, faaModeIndicator)) {
            logger << logWarning << talker << " GLL message with bad FAA mode indicator '"
                   << faaModeIndicatorStr << "'" << eol;
            return false;
        }
    } else {
        faaModeIndicator = FAA_MODE_NONE;
    }

    return true;
}

void NMEAGLLMessage::print() {
    Serial.print("GPS GLL: ");

    latitude.print();

    Serial.print(" ");
    longitude.print();

    Serial.print(" ");
    time.print();

    Serial.print(" ");
    printNMEADataValid(dataValid);

    if (faaModeIndicator != FAA_MODE_NONE) {
        Serial.print(" ");
        printNMEAFAAModeIndicator(faaModeIndicator);
    }

    Serial.println();
}

NMEAGLLMessage *parseNMEAGLLMessage(NMEATalker talker, NMEALine &nmeaLine) {
    NMEAGLLMessage *message = new(nmeaMessageBuffer) NMEAGLLMessage;
    if (!message) {
        return NULL;
    }

    if (!message->parse(talker, nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}