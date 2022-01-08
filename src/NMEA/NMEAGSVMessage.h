#ifndef NMEA_GSV_MESSAGE_H
#define NMEA_GSV_MESSAGE_H

#include <Arduino.h>

#include "NMEAMessage.h"
#include "NMEAUInt8.h"
#include "NMEAGSVSatelitteInfo.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAGSVMessage : public NMEAMessage {
    private:
        NMEAUInt8 sentencesInGroup;
        NMEAUInt8 sentenceNumber;
        NMEAUInt8 numberSatellites;
        uint8_t satelittesInMessage;
        NMEAGSVSatelitteInfo satelittes[4];

    public:
        NMEAGSVMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() override;
        virtual void log() override;

    friend class NMEADataModelBridge;
};

extern NMEAGSVMessage *parseNMEAGSVMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
