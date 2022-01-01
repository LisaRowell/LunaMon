#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAGLLMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"

#include "Util/Logger.h"

NMEAMessage::NMEAMessage(NMEATalker &talker) : talker(talker) {
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
        case NMEA_MSG_TYPE_GLL:
            return parseNMEAGLLMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_RMC:
            return parseNMEARMCMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_TXT:
            return parseNMEATXTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_UNKNOWN:
        default:
            logger << logWarning << "Unknown NMEA message type (" << msgTypeStr << ") from "
                   << talker << eol;
            return NULL;
    }
}
