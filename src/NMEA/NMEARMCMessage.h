#ifndef NMEA_RMC_MESSAGE_H
#define NMEA_RMC_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATenthsUInt16.h"
#include "NMEADirection.h"
#include "NMEADate.h"
#include "NMEAMagneticVariation.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEARMCMessage : public NMEAMessage {
    private:
        NMEATime time;
        NMEADataValid dataValid;
        NMEALatitude latitude;
        NMEALongitude longitude;
        NMEATenthsUInt16 speedOverGround;
        NMEADirection trackMadeGood;
        NMEADate date;
        NMEAMagneticVariation magneticVariation;
        NMEAFAAModeIndicator faaModeIndicator;

    public:
        NMEARMCMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEARMCMessage *parseNMEARMCMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
