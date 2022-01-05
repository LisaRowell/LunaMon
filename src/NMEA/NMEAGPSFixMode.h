#ifndef NMEA_GPS_FIX_MODE_H
#define NMEA_GPS_FIX_MODE_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEAGPSFixMode : public LoggableItem {
    private:
        enum GPSFixMode {
            GPS_FIX_MODE_NO_FIX = 1,
            GPS_FIX_MODE_2D = 2,
            GPS_FIX_MODE_3D = 3
        };
        GPSFixMode gpsFixMode;

    public:
        bool set(String &gpsQualityStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
