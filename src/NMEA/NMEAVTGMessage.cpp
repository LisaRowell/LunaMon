#include <Arduino.h>

#include "NMEAVTGMessage.h"
#include "NMEASpeed.h"
#include "NMEAHeading.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAVTGMessage::NMEAVTGMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAVTGMessage::parse(NMEALine &nmeaLine) {
    if (!trackMadeGood.extract(nmeaLine, talker, "VTG")) {
        return false;
    }

    // There are two forms of this sentence, one with "T", "M", "N", "K" after each field, and one
    // with out.
    String secondWord;
    if (!nmeaLine.extractWord(secondWord)) {
        logger << logError << talker << " VTG message missing second field" << eol;
        return false;
    }

    bool oldForm;
    if (secondWord != "T") {
        oldForm = true;
    } else {
        oldForm = false;
        if (!nmeaLine.extractWord(secondWord)) {
            logger << logError << talker << " VTG message missing Course Over Ground, Magnetic"
                   << eol;
            return false;
        }
    }

    if (!courseOverGroundMagnetic.set(secondWord)) {
        logger << logError << talker << " VTG message with a bad Course Over Ground, Magentic"
               << eol;
        return false;
    }

    if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "M")) {
            return false;
        }
    }

    if (!speedOverGround.extract(nmeaLine, talker, "VTG")) {
        return false;
    }

   if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "N")) {
            return false;
        }
    }

    if (!speedOverGroundKm2.extract(nmeaLine, talker, "VTG")) {
        return false;
    }

   if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "K")) {
            return false;
        }
    }

    String faaModeIndicatorStr;
    if (nmeaLine.extractWord(faaModeIndicatorStr)) {
        if (!faaModeIndicator.set(faaModeIndicatorStr)) {
            logger << logWarning << talker << " VTG message with bad FAA Mode Indicator field '"
                   << faaModeIndicatorStr << "'" << eol;
            return false;
        }
    }


    return true;
}

bool NMEAVTGMessage::extractConstantWord(NMEALine &nmeaLine, const char *constantWord) {
    String word;
    if (!nmeaLine.extractWord(word)) {
        logger << logError << talker << " VTG message missing " << constantWord << " field" << eol;
        return false;
    }

    if (word != constantWord) {
        logger << logError << talker << " VTG message with bad " << constantWord << " field" << eol;
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAVTGMessage::type() {
    return NMEA_MSG_TYPE_VTG;
}

void NMEAVTGMessage::log() {
    logger << logDebug << talker << " VTG: " << trackMadeGood << " " << courseOverGroundMagnetic
           << " " << speedOverGround << " ";

    speedOverGroundKm2.log(logger, "km/h");

    if (faaModeIndicator.hasValue()) {
        logger << " " << faaModeIndicator;
    }
    logger << eol;
}

NMEAVTGMessage *parseNMEAVTGMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAVTGMessage *message = new(nmeaMessageBuffer) NMEAVTGMessage(talker);
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
