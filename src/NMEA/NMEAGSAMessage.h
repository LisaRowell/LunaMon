#ifndef NMEA_GSA_MESSAGE_H
#define NMEA_GSA_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAGPSFixMode.h"
#include "NMEAUInt16.h"
#include "NMEAHundredthsUInt8.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAGSAMessage : public NMEAMessage {
    private:
        bool automaticMode;
        NMEAGPSFixMode gpsFixMode;
        NMEAUInt16 satelliteIDs[12];
        NMEAHundredthsUInt8 pdop;
        NMEAHundredthsUInt8 hdop;
        NMEAHundredthsUInt8 vdop;

    public:
        NMEAGSAMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEAGSAMessage *parseNMEAGSAMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
