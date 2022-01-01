#ifndef NMEA_SPEED_H
#define NMEA_SPEED_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEASpeed : public LoggableItem {
    private:
        uint16_t speed;
        uint8_t tenths;
        bool hasValue;

    public:
        bool set(const String &speedStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
        void log(Logger &logger, const char *units) const;
};

#endif
