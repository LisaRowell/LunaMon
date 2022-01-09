#ifndef NMEA_GGA_MESSAGE_H
#define NMEA_GGA_MESSAGE_H

#include <Arduino.h>

#include "NMEAMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEAGPSQuality.h"
#include "NMEAHundredthsUInt16.h"
#include "NMEATenthsInt16.h"
#include "NMEATenthsUInt16.h"
#include "NMEAUInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAGGAMessage : public NMEAMessage {
    private:
        NMEATime time;
        NMEALatitude latitude;
        NMEALongitude longitude;
        NMEAGPSQuality gpsQuality;
        NMEAUInt16 numberSatellites;
        NMEAHundredthsUInt16 horizontalDilutionOfPrecision;
        NMEATenthsInt16 antennaAltitude;
        NMEATenthsInt16 geoidalSeparation;
        NMEATenthsUInt16 gpsDataAge;
        NMEAUInt16 differentialReferenceStation;

    public:
        NMEAGGAMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEAGGAMessage *parseNMEAGGAMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
