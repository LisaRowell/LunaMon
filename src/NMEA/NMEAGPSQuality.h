#ifndef NMEA_GPS_QUALITY_H
#define NMEA_GPS_QUALITY_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

class NMEAGPSQuality : public LoggableItem {
    private:
        enum GPSQuality {
            GPS_QUALITY_FIX_NOT_AVAILABLE = 0,
            GPS_QUALITY_GPS_FIX = 1,
            GPS_QUALITY_DIFFERENTIAL_GPS_FIX = 2,
            GPS_QUALITY_PPS_FIX = 3,
            GPS_QUALITY_REAL_TIME_KINEMATIC = 4,
            GPS_QUALITY_FLOAT_RTK = 5,
            GPS_QUALITY_ESTIMATED = 6,
            GPS_QUALITY_MANUAL_INPUT_MODE = 7,
            GPS_QUALITY_SIMULATED_MODE = 8
        };
        GPSQuality gpsQuality;

        bool set(etl::string_view &gpsQualityView);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
