#ifndef NMEA_LATITUDE_H
#define NMEA_LATITUDE_H

#include "NMEACoordinate.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

enum NorthOrSouth {
    NORTH,
    SOUTH
};

class NMEALatitude : public NMEACoordinate, public LoggableItem {
    private:
        enum NorthOrSouth northOrSouth;

    public:
        bool set(const String &string, const String &northOrSouthStr);
        void publish(DataModelLeaf &leaf);
        virtual void log(Logger &logger) const override;
};

#endif
