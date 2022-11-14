#ifndef NMEA_DATE_H
#define NMEA_DATE_H

#include "NMEA/NMEALine.h"
#include "NMEA/NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"
#include "Util/LoggableItem.h"

#include <Arduino.h>

class NMEADate : public LoggableItem {
    private:
        bool hasValue;
        uint16_t year;
        uint8_t month;
        uint8_t day;

    public:
        bool set(const String &dateStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
