#include <Arduino.h>

#include "NMEAVDMMessage.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAUInt8.h"
#include "NMEAUInt16.h"
#include "NMEARadioChannelCode.h"
#include "NMEAMsgType.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAVDMMessage::NMEAVDMMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAVDMMessage::parse(NMEALine &nmeaLine) {
    if (!nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " VDM message in unsupported, non-encapsulated format"
               << eol;
        return false;
    }

    if (!totalFragments.extract(nmeaLine, talker, "VDM", "Total Fragments")) {
        return false;
    }

    if (!fragmentNumber.extract(nmeaLine, talker, "VDM", "Fragment Number")) {
        return false;
    }

    if (!messageId.extract(nmeaLine, talker, "VDM", "Message Id", true)) {
        return false;
    }

    if (!radioChannelCode.extract(nmeaLine, talker, "VDM")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAVDMMessage::type() {
    return NMEA_MSG_TYPE_VDM;
}

void NMEAVDMMessage::log() {
    logger << logDebugNMEA << talker << " VDM: " << fragmentNumber << " of " << totalFragments
           << " Msg Id " << messageId << " Channel " << radioChannelCode << eol;
}

NMEAVDMMessage *parseNMEAVDMMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAVDMMessage *message = new (nmeaMessageBuffer)NMEAVDMMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
