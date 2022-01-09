#ifndef NMEA_GLL_MESSAGE_H
#define NMEA_GLL_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEAFAAModeIndicator.h"

class NMEAGLLMessage : public NMEAMessage {
    private:
        NMEALatitude latitude;
        NMEALongitude longitude;
        NMEATime time;
        NMEADataValid dataValid;
        NMEAFAAModeIndicator faaModeIndicator;

    public:
        NMEAGLLMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEAGLLMessage *parseNMEAGLLMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
