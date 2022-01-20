#include "NMEAVTGMessage.h"
#include "NMEATenthsUInt16.h"
#include "NMEADirection.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/Logger.h"

#include <Arduino.h>

NMEAVTGMessage::NMEAVTGMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAVTGMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " VTG message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!trackMadeGood.extract(nmeaLine, talker, "VTG", "Track Made Good")) {
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

    if (!courseOverGroundMagnetic.set(secondWord, true)) {
        logger << logError << talker << " VTG message with a bad Course Over Ground, Magentic"
               << eol;
        return false;
    }

    if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "VTG", "M")) {
            return false;
        }
    }

    if (!speedOverGround.extract(nmeaLine, talker, "VTG", "Speed Over Ground")) {
        return false;
    }

   if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "VTG", "N")) {
            return false;
        }
    }

    if (!speedOverGroundKm2.extract(nmeaLine, talker, "VTG", "Speed Over Ground km/s")) {
        return false;
    }

   if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "VTG", "K")) {
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

enum NMEAMsgType NMEAVTGMessage::type() const {
    return NMEA_MSG_TYPE_VTG;
}

void NMEAVTGMessage::log() const {
    logger << logDebugNMEA << talker << " VTG: " << trackMadeGood << " " << courseOverGroundMagnetic
           << " " << speedOverGround << "kn " << speedOverGroundKm2 << "km/h";

    if (faaModeIndicator.hasValue()) {
        logger << " " << faaModeIndicator;
    }
    logger << eol;
}

NMEAVTGMessage *parseNMEAVTGMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAVTGMessage *message = new (nmeaMessageBuffer)NMEAVTGMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
