#ifndef NMEA_VTG_MESSAGE_H
#define NMEA_VTG_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAHeading.h"
#include "NMEASpeed.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAVTGMessage : public NMEAMessage {
    private:
        NMEAHeading trackMadeGood;
        NMEAHeading courseOverGroundMagnetic;
        NMEASpeed speedOverGround;
        NMEASpeed speedOverGroundKm2;
        NMEAFAAModeIndicator faaModeIndicator;

        bool extractConstantWord(NMEALine &nmeaLine, const char *constantWord);

    public:
        NMEAVTGMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() override;
        virtual void log() override;

    friend class NMEADataModelBridge;
};

extern NMEAVTGMessage *parseNMEAVTGMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
