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
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " GLL message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!latitude.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!longitude.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!time.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!dataValid.extract(nmeaLine, talker, "GLL")) {
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
    NMEAGLLMessage *message = new (nmeaMessageBuffer)NMEAGLLMessage(talker);
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
