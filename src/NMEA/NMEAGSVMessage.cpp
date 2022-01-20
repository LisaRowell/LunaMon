#include "NMEAGSVMessage.h"
#include "NMEAMsgType.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAUInt8.h"
#include "NMEAMessageBuffer.h"

#include "Util/StringTools.h"
#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAGSVMessage::NMEAGSVMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGSVMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " GSV message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!sentencesInGroup.extract(nmeaLine, talker, "GSV", "Sentences in Group", false, 9)) {
        return false;
    }

    if (!sentenceNumber.extract(nmeaLine, talker, "GSV", "Sentence Number", false, 9)) {
        return false;
    }

    if (!numberSatellites.extract(nmeaLine, talker, "GSV", "Number Satellites")) {
        return false;
    }

    bool endOfInput;
    for (satelittesInMessage = 0, endOfInput = false; !endOfInput && satelittesInMessage < 4;) {
        if (!satelittes[satelittesInMessage].extract(nmeaLine, talker, endOfInput)) {
            return false;
        }
        if (!endOfInput) {
            satelittesInMessage++;
        }
    }

    return true;
}

enum NMEAMsgType NMEAGSVMessage::type() const {
    return NMEA_MSG_TYPE_GSV;
}

void NMEAGSVMessage::log() const {
    logger << logDebugNMEA << talker << " GSV: " << sentenceNumber << " of " << sentencesInGroup
           << " Satelllites " << numberSatellites;

    unsigned satelitteIndex;
    for (satelitteIndex = 0; satelitteIndex < satelittesInMessage; satelitteIndex++) {
        logger << " " << satelittes[satelitteIndex];
    }

    logger << eol;
}

NMEAGSVMessage *parseNMEAGSVMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGSVMessage *message = new (nmeaMessageBuffer)NMEAGSVMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
