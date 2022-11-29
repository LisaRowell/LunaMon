#ifndef NMEA_COORDINATE_H
#define NMEA_COORDINATE_H

#include "DataModel/DataModelStringLeaf.h"

#include <etl/string_view.h>

#include <stdint.h>

class NMEACoordinate {
    protected:
        uint8_t degrees;
        float minutes;

        bool setDegrees(const etl::string_view &degreesView, uint8_t maxDegrees);
        bool setMinutes(const etl::string_view &minutesView);
        void snprint(char *string, size_t maxLength) const;
        void publish(DataModelStringLeaf &leaf, bool isPositive) const;
};

#endif
