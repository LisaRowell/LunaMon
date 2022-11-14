#ifndef NMEA_UINT8_H
#define NMEA_UINT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelUInt8Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <Arduino.h>

class NMEAUInt8 : public LoggableItem {
    private:
        uint8_t value;
        bool valuePresent;

    public:
        bool set(const String &valueStr, bool optional, uint8_t maxValue);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, uint8_t maxValue = 0xff);
        bool hasValue() const;
        void publish(DataModelUInt8Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
