#ifndef NMEA_INT8_H
#define NMEA_INT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEAInt8 : public LoggableItem {
    private:
        int8_t value;
        bool valuePresent;

    public:
        bool set(const String &valueStr, bool optional, int8_t minValue, int8_t maxValue);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, int8_t minValue = -128,
                     int8_t maxValue = 127);
        bool hasValue() const;
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
