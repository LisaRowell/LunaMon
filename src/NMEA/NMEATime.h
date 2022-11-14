#ifndef NMEA_TIME_H
#define NMEA_TIME_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <Arduino.h>

class NMEATime : public LoggableItem {
    private:
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t secondPrecision;
        uint32_t secondFraction;

        void buildSecondsFactionString(char *string) const;

    public:
        bool set(const String &timeStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
