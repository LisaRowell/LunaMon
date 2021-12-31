#ifndef NMEA_TXT_MESSAGE_H
#define NMEA_TXT_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEATXTMessage : public NMEAMessage {
    private:
        uint8_t totalSentences;
        uint8_t sentenceNumber;
        uint8_t textIdentifier;
        char text[maxNMEALineLength];

    public:
        NMEATXTMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() override;
        virtual void log() override;
};

extern NMEATXTMessage *parseNMEATXTMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
