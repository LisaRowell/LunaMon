#ifndef NMEA_INT8_H
#define NMEA_INT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelInt8Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

#include <stdint.h>

class NMEAInt8 : public LoggableItem {
    private:
        int8_t value;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional, int8_t minValue,
                 int8_t maxValue);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, int8_t minValue = -128,
                     int8_t maxValue = 127);
        bool hasValue() const;
        void publish(DataModelInt8Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
