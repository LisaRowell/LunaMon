#ifndef NMEA_HEADING_H
#define NMEA_HEADING_H

#include <Arduino.h>

#include "NMEA/NMEALine.h"
#include "NMEA/NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"
#include "Util/LoggableItem.h"

class NMEAHeading : public LoggableItem {
    private:
        uint16_t heading;
        uint8_t tenths;
        bool hasValue;

    public:
        bool set(const String &headingStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
