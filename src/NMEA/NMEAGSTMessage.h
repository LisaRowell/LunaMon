#ifndef NMEA_GST_MESSAGE_H
#define NMEA_GST_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATime.h"
#include "NMEATenthsUInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAGSTMessage : public NMEAMessage {
    private:
        NMEATime time;
        NMEATenthsUInt16 standardDeviationOfRangeInputsRMS;
        NMEATenthsUInt16 standardDeviationOfSemiMajorAxis;
        NMEATenthsUInt16 standardDeviationOfSemiMinorAxis;
        NMEATenthsUInt16 orientationOfSemiMajorAxis;
        NMEATenthsUInt16 standardDeviationOfLatitudeError;
        NMEATenthsUInt16 standardDeviationOfLongitudeError;
        NMEATenthsUInt16 standardDeviationOfAltitudeError;

    public:
        NMEAGSTMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEAGSTMessage *parseNMEAGSTMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
