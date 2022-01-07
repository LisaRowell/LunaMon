#include <Arduino.h>

#include "NMEAGSAMessage.h"
#include "NMEAGPSFixMode.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Util/StringTools.h"
#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAGSAMessage::NMEAGSAMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGSAMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " GSA message in unsupported encapsulated format" << eol;
        return false;
    }

    String manualOrAutomaticModeStr;
    if (!nmeaLine.extractWord(manualOrAutomaticModeStr)) {
        logger << logWarning << talker << " GSA message missing Manual or Automatic Mode indicator"
               << eol;
        return false;
    }
    if (manualOrAutomaticModeStr == "A") {
        automaticMode = true;
    } else if (manualOrAutomaticModeStr == "M") {
        automaticMode = false;
    } else {
        logger << logWarning << talker << " GSA message with bad Manual or Automatic Mode indicator"
               << eol;
        return false;
    }

    if (!gpsFixMode.extract(nmeaLine, talker, "GSA")) {
        return false;
    }

    unsigned satelliteIndex;
    for (satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        if (!satelliteIDs[satelliteIndex].extract(nmeaLine, talker, "GSA", "Satellite ID", true)) {
            return false;
        }
    }

    if (!pdop.extract(nmeaLine, talker, "GSA", "PDOP")) {
        return false;
    }
    if (!hdop.extract(nmeaLine, talker, "GSA", "HDOP")) {
        return false;
    }
    if (!vdop.extract(nmeaLine, talker, "GSA", "VDOP")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAGSAMessage::type() {
    return NMEA_MSG_TYPE_GSA;
}

void NMEAGSAMessage::log() {
    logger << logDebugNMEA << talker << " GSA: ";

    if (automaticMode) {
        logger << "Automatic ";
    } else {
        logger << "Manual ";
    }

    logger << gpsFixMode << " Satellites ";

    unsigned satelliteIndex;
    for (satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        logger << satelliteIDs[satelliteIndex] << " ";
    }
 
    logger << "PDOP " << pdop << " HDOP " << hdop << " VDOP " << vdop << eol;
}

NMEAGSAMessage *parseNMEAGSAMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGSAMessage *message = new (nmeaMessageBuffer)NMEAGSAMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
