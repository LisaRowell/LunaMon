#ifndef NMEA_GLL_MESSAGE_H
#define NMEA_GLL_MESSAGE_H

#include "NMEAMessage.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Time.h"
#include "NMEAFAAModeIndicator.h"

class NMEAGLLMessage : public NMEAMessage {
    private:
        Latitude latitude;
        Longitude longitude;
        Time time;
        bool dataValid;
        enum NMEAFAAModeIndicator faaModeIndicator;

    public:
        bool parse(NMEATalker talker, NMEALine &nmeaLine);
        void print() override;
};

extern NMEAGLLMessage *parseNMEAGLLMessage(NMEATalker talker, NMEALine &nmeaLine);

#endif