#ifndef NMEA_DIRECTION_H
#define NMEA_DIRECTION_H

#include <Arduino.h>

#include "NMEA/NMEALine.h"
#include "NMEA/NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"
#include "Util/LoggableItem.h"

class NMEADirection : public LoggableItem {
    private:
        uint16_t heading;
        uint8_t tenths;
        bool valuePresent;

    public:
        bool set(const String &headingStr, bool optional);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
