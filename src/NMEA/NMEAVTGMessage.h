#ifndef NMEA_VTG_MESSAGE_H
#define NMEA_VTG_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATenthsUInt16.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAVTGMessage : public NMEAMessage {
    private:
        NMEATenthsUInt16 trackMadeGood;
        NMEATenthsUInt16 courseOverGroundMagnetic;
        NMEATenthsUInt16 speedOverGround;
        NMEATenthsUInt16 speedOverGroundKm2;
        NMEAFAAModeIndicator faaModeIndicator;

        bool wordIsT(const etl::string_view &word);

    public:
        NMEAVTGMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEAVTGMessage *parseNMEAVTGMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
