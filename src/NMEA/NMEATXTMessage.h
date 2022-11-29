#ifndef NMEA_TXT_MESSAGE_H
#define NMEA_TXT_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEALine.h"

#include <etl/string.h>

#include <stdint.h>

class NMEATXTMessage : public NMEAMessage {
    private:
        uint8_t totalSentences;
        uint8_t sentenceNumber;
        uint8_t textIdentifier;
        etl::string<maxNMEALineLength> text;

        bool getTwoDigitField(NMEALine &nmeaLine, uint8_t &value, const char *fieldName);

    public:
        NMEATXTMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;
};

extern NMEATXTMessage *parseNMEATXTMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
