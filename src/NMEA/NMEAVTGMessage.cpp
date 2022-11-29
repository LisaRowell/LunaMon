#include "NMEAVTGMessage.h"
#include "NMEATenthsUInt16.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

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
    etl::string_view secondWordView;
    if (!nmeaLine.getWord(secondWordView)) {
        logger << logError << talker << " VTG message missing second field" << eol;
        return false;
    }

    bool oldForm = !wordIsT(secondWordView);
    if (!oldForm) {
        if (!nmeaLine.getWord(secondWordView)) {
            logger << logError << talker << " VTG message missing Course Over Ground, Magnetic "
                   << eol;
            return false;
        }
    }

    if (!courseOverGroundMagnetic.set(secondWordView, true)) {
        logger << logError << talker << " VTG message with a bad Course Over Ground, Magentic"
               << "'" << secondWordView << "'" << eol;
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

    if (!faaModeIndicator.extract(nmeaLine, talker, "VTG")) {
        return false;
    }

    return true;
}

bool NMEAVTGMessage::wordIsT(const etl::string_view &word) {
    if (word.size() == 1) {
        if (word.front() == 'T') {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
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
