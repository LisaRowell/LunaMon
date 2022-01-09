#ifndef NMEA_MESSAGE_H
#define NMEA_MESSAGE_H

#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"

class NMEAMessage {
    protected:
        NMEATalker talker;

        bool extractConstantWord(NMEALine &nmeaLine, const char *messageType,
                                 const char *constantWord);

    public:
        NMEAMessage(NMEATalker &talker);
        NMEATalker source() const;
        virtual enum NMEAMsgType type() const = 0;
        virtual void log() const = 0;
};

NMEAMessage *parseNMEAMessage(NMEALine &nmeaLine);

#endif
