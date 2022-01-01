#ifndef NMEA_COORDINATE_H
#define NMEA_COORDINATE_H

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEACoordinate {
    protected:
        uint8_t degrees;
        float minutes;

        bool setDegrees(const String &string, unsigned startDegrees, unsigned endDegrees,
                        uint8_t maxDegrees);
        bool setMinutes(const String &string, unsigned startMinutes);
        void snprint(char *string, size_t maxLength) const;
        void publish(DataModelLeaf &leaf, bool isPositive) const;
};

#endif
