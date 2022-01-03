#include <Arduino.h>

#include "NMEARMCMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATenthsUInt16.h"
#include "NMEAHeading.h"
#include "NMEADate.h"
#include "NMEAMagneticVariation.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEARMCMessage::NMEARMCMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEARMCMessage::parse(NMEALine &nmeaLine) {
    if (!time.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!dataValid.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!latitude.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!longitude.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!speedOverGround.extract(nmeaLine, talker, "RMC", "Speed Over Ground")) {
        return false;
    }

    if (!trackMadeGood.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!date.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!magneticVariation.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    String faaModeIndicatorStr;
    if (nmeaLine.extractWord(faaModeIndicatorStr)) {
        if (!faaModeIndicator.set(faaModeIndicatorStr)) {
            logger << logWarning << talker << " RMC message with bad FAA Mode Indicator field '"
                   << faaModeIndicatorStr << "'" << eol;
            return false;
        }
    }

    return true;
}

enum NMEAMsgType NMEARMCMessage::type() {
    return NMEA_MSG_TYPE_RMC;
}

void NMEARMCMessage::log() {
    logger << logDebug << talker << " RMC: " << time << " " << dataValid << " " << latitude << " "
           << longitude << " " << speedOverGround << "kn " << trackMadeGood << " " << date << " "
           << magneticVariation;
    if (faaModeIndicator.hasValue()) {
        logger << " " << faaModeIndicator;
    }
    logger << eol;
}

NMEARMCMessage *parseNMEARMCMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEARMCMessage *message = new (nmeaMessageBuffer)NMEARMCMessage(talker);
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
