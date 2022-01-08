#ifndef NMEA_GSV_SATELLITE_INFO_H
#define NMEA_GSV_SATELLITE_INFO_H

#include "NMEAUInt16.h"
#include "NMEAInt8.h"
#include "NMEADirection.h"
#include "NMEAUInt8.h"
#include "NMEALine.h"

#include "Util/LoggableItem.h"

class NMEAGSVSatelitteInfo : public LoggableItem {
    private:
        NMEAUInt16 id;
        NMEAInt8 elevation;
        NMEADirection azimuth;
        NMEAUInt8 signalToNoiseRatio;

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, bool &endOfInput);
        virtual void log(Logger &logger) const override;

    friend class NMEADataModelBridge;
};

#endif
