#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAGLLMessage.h"
#include "Util/Logger.h"

void parseNMEAMessage(NMEALine &nmeaLine) {
    String tag;
    if (!nmeaLine.extractWord(tag)) {
        logger << logWarning << "NMEA message missing tag" << eol;
        return;
    }
    if (tag.length() != 5) {
        logger << logWarning << "Bad NMEA tag" << eol;
        return;
    }

    String talkerCode = tag.substring(0, 2);
    NMEATalker talker(talkerCode);

    String msgTypeStr = tag.substring(2, 5);
    enum NMEAMsgType msgType = parseNMEAMsgType(msgTypeStr);

    switch (msgType) {
        case NMEA_MSG_TYPE_GLL:
            parseNMEAGLLMessage(talker, nmeaLine);
            break;

        case NMEA_MSG_TYPE_UNKNOWN:
        default:
            logger << logWarning << "Unknown NMEA message type (" << msgTypeStr << ") from "
                   << talker << eol;
    }
}
