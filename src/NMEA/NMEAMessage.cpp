#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAGLLMessage.h"
#include "Util/Logger.h"

void NMEAMessage::setTalker(enum NMEATalker talker) {
  NMEAMessage::talker = talker;
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
  enum NMEATalker talker = parseNMEATalker(talkerCode);

  String msgTypeStr = tag.substring(2, 5);
  enum NMEAMsgType msgType = parseNMEAMsgType(msgTypeStr);

  NMEAMessage *message;
  switch (msgType) {
    case NMEA_MSG_TYPE_GLL:
      message = parseNMEAGLLMessage(talker, nmeaLine);
      if (!message) {
        return NULL;
      }
      break;

    case NMEA_MSG_TYPE_UNKNOWN:
    default:
      logger << logWarning << "Unknown NMEA message type (" << msgTypeStr << ") from " << talker
             << eol;
      return NULL;
  }

  message->setTalker(talker);
  return message;
}