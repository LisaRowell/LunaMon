#ifndef NMEA_VTG_MESSAGE_H
#define NMEA_VTG_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEADirection.h"
#include "NMEATenthsUInt16.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAVTGMessage : public NMEAMessage {
    private:
        NMEADirection trackMadeGood;
        NMEADirection courseOverGroundMagnetic;
        NMEATenthsUInt16 speedOverGround;
        NMEATenthsUInt16 speedOverGroundKm2;
        NMEAFAAModeIndicator faaModeIndicator;

    public:
        NMEAVTGMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() override;
        virtual void log() override;

    friend class NMEADataModelBridge;
};

extern NMEAVTGMessage *parseNMEAVTGMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
