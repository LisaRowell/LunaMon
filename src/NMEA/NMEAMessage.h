#ifndef NMEA_MESSAGE_H
#define NMEA_MESSAGE_H

#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"

class NMEAMessage {
    protected:
        NMEATalker talker;

    public:
        NMEAMessage(NMEATalker &talker);
        virtual enum NMEAMsgType type() = 0;
        virtual void log() = 0;
};

NMEAMessage *parseNMEAMessage(NMEALine &nmeaLine);

#endif
