#ifndef NMEA_UINT16_H
#define NMEA_UINT16_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <Arduino.h>

class NMEAUInt16 : public LoggableItem {
    private:
        uint16_t value;
        bool valuePresent;

    public:
        bool set(const String &valueStr, bool optional, uint16_t maxValue);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, uint16_t maxValue = 0xffff);
        bool hasValue() const;
        uint16_t getValue() const;
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
