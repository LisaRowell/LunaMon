#ifndef NMEA_LONGITUDE_H
#define NMEA_LONGITUDE_H

#include "NMEACoordinate.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

enum EastOrWest {
    EAST,
    WEST
};

class NMEALongitude : public NMEACoordinate, public LoggableItem {
    private:
        enum EastOrWest eastOrWest;

    public:
        bool set(const String &string, const String &eastOrWestStr);
        void publish(DataModelLeaf &leaf);
        virtual void log(Logger &logger) const override;
};

#endif
