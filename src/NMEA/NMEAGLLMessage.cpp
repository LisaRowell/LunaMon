#include <Arduino.h>

#include "NMEAGLLMessage.h"
#include "NMEAMsgType.h"
#include "NMEAMessage.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAGLLMessage::NMEAGLLMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGLLMessage::parse(NMEALine &nmeaLine) {
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
        logger << logWarning << talker << " GLL message with bad latitude '" << latitudeStr << "' '"
               << northOrSouthStr << "'" << eol;
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
        logger << logWarning << talker << " GLL message with bad longitude '" << longitudeStr
               << "' '" << eastOrWestStr << "'" << eol;
        return false;
    }

    String timeStr;
    if (!nmeaLine.extractWord(timeStr)) {
        logger << logWarning << talker << " GLL message missing Time field" << eol;
        return false;
    }
    if (!time.set(timeStr)) {
        logger << logWarning << talker << " GLL message with bad Time field '" << timeStr << "'"
               << eol;
        return false;
    }

    String dataValidStr;
    if (!nmeaLine.extractWord(dataValidStr)) {
        logger << logWarning << talker << " GLL message missing Data Valid field" << eol;
        return false;
    }
    if (!dataValid.set(dataValidStr)) {
        logger << logWarning << talker << " GLL message with bad Data Valid field '" << dataValidStr
               << "'" << eol;
        return false;
    }

    String faaModeIndicatorStr;
    if (nmeaLine.extractWord(faaModeIndicatorStr)) {
        if (!faaModeIndicator.set(faaModeIndicatorStr)) {
            logger << logWarning << talker << " GLL message with bad FAA Mode Indicator field '"
                   << faaModeIndicatorStr << "'" << eol;
            return false;
        }
    }

    return true;
}

enum NMEAMsgType NMEAGLLMessage::type() {
    return NMEA_MSG_TYPE_GLL;
}

void NMEAGLLMessage::log() {
    logger << logDebug << talker << " GLL: " << latitude << " " << longitude << " " << time << " "
           << dataValid;
    if (faaModeIndicator.hasValue()) {
        logger << " " << faaModeIndicator;
    }
    logger << eol;
}

NMEAGLLMessage *parseNMEAGLLMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGLLMessage *message = new(nmeaMessageBuffer) NMEAGLLMessage(talker);
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
