#ifndef NMEA_UINT8_H
#define NMEA_UINT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelUInt8Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

#include <stdint.h>

class NMEAUInt8 : public LoggableItem {
    private:
        uint8_t value;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional, uint8_t maxValue);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, uint8_t maxValue = 0xff);
        bool hasValue() const;
        void publish(DataModelUInt8Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
