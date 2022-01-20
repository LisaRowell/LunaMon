#include "NMEATXTMessage.h"
#include "NMEAMsgType.h"
#include "NMEAMessage.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAMessageBuffer.h"

#include "Util/PlacementNew.h"
#include "Util/StringTools.h"
#include "Util/Logger.h"

#include <Arduino.h>

NMEATXTMessage::NMEATXTMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEATXTMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " TXT message in unsupported encapsulated format" << eol;
        return false;
    }

    String totalSentencesStr;
    if (!nmeaLine.extractWord(totalSentencesStr)) {
        logger << logWarning << talker << " TXT message missing Total Sentences field" << eol;
        return false;
    }
    if (!convertTwoDigitDecimalString(totalSentencesStr, totalSentences)) {
        logger << logWarning << talker << " TXT message with bad Total Sentences field '"
               << totalSentencesStr << "'" << eol;
        return false;
    }

    String sentenceNumberStr;
    if (!nmeaLine.extractWord(sentenceNumberStr)) {
        logger << logWarning << talker << " TXT message missing Sentence Number field" << eol;
        return false;
    }
    if (!convertTwoDigitDecimalString(sentenceNumberStr, sentenceNumber)) {
        logger << logWarning << talker << " TXT message with bad Sentence Number field '"
               << sentenceNumberStr << "'" << eol;
        return false;
    }

    String textIdentifierStr;
    if (!nmeaLine.extractWord(textIdentifierStr)) {
        logger << logWarning << talker << " TXT message missing Text Identifier field" << eol;
        return false;
    }
    if (!convertTwoDigitDecimalString(textIdentifierStr, textIdentifier)) {
        logger << logWarning << talker << " TXT message with bad Text Identifier field '"
               << textIdentifierStr << "'" << eol;
        return false;
    }

    String textStr;
    if (!nmeaLine.extractWord(textStr)) {
        logger << logWarning << talker << " TXT message missing Text field" << eol;
        return false;
    }
    strncpy(text, textStr.c_str(), maxNMEALineLength);

    return true;
}

enum NMEAMsgType NMEATXTMessage::type() const {
    return NMEA_MSG_TYPE_TXT;
}

void NMEATXTMessage::log() const {
    logger << logDebugNMEA << talker << " TXT: TotalSentences " << totalSentences << " Sentence "
           << sentenceNumber << " TextId " << textIdentifier << " " << text << eol;
}

NMEATXTMessage *parseNMEATXTMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEATXTMessage *message = new (nmeaMessageBuffer)NMEATXTMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
