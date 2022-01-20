#ifndef NMEA_LONGITUDE_H
#define NMEA_LONGITUDE_H

#include "NMEACoordinate.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <Arduino.h>

enum EastOrWest {
    EAST,
    WEST
};

class NMEALongitude : public NMEACoordinate, public LoggableItem {
    private:
        enum EastOrWest eastOrWest;

    public:
        bool set(const String &string, const String &eastOrWestStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
