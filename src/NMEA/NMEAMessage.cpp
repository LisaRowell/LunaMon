#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVTGMessage.h"

#include "Util/Logger.h"

NMEAMessage::NMEAMessage(NMEATalker &talker) : talker(talker) {
}

bool NMEAMessage::extractConstantWord(NMEALine &nmeaLine, const char *messageType,
                                      const char *constantWord) {
    String word;
    if (!nmeaLine.extractWord(word)) {
        logger << logError << talker << " " << messageType << " message missing " << constantWord
               << " field" << eol;
        return false;
    }

    if (word != constantWord) {
        logger << logError << talker << " " << messageType << " message with bad " << constantWord
               << " field" << eol;
        return false;
    }

    return true;
}

NMEAMessage *parseNMEAMessage(NMEALine &nmeaLine) {
    String tag;
    if (!nmeaLine.extractWord(tag)) {
        logger << logWarning << "NMEA message missing tag" << eol;
        return NULL;
    }
    if (tag.length() != 5) {
        logger << logWarning << "Bad NMEA tag" << eol;
        return NULL;
    }

    String talkerCode = tag.substring(0, 2);
    NMEATalker talker(talkerCode);

    String msgTypeStr = tag.substring(2, 5);
    enum NMEAMsgType msgType = parseNMEAMsgType(msgTypeStr);

    switch (msgType) {
        case NMEA_MSG_TYPE_GGA:
            return parseNMEAGGAMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GLL:
            return parseNMEAGLLMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GSA:
            return parseNMEAGSAMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_RMC:
            return parseNMEARMCMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_TXT:
            return parseNMEATXTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_VTG:
            return parseNMEAVTGMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_UNKNOWN:
        default:
            logger << logWarning << "Unknown NMEA message type (" << msgTypeStr << ") from "
                   << talker << eol;
            return NULL;
    }
}
