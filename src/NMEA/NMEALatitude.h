#ifndef NMEA_LATITUDE_H
#define NMEA_LATITUDE_H

#include "NMEACoordinate.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <Arduino.h>

enum NorthOrSouth {
    NORTH,
    SOUTH
};

class NMEALatitude : public NMEACoordinate, public LoggableItem {
    private:
        enum NorthOrSouth northOrSouth;

    public:
        bool set(const String &string, const String &northOrSouthStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
