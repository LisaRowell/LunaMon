#ifndef NMEA_TENTHS_INT16_H
#define NMEA_TENTHS_INT16_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelTenthsInt16Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

#include <stdint.h>

class NMEATenthsInt16 : public LoggableItem {
    private:
        int16_t integer;
        uint8_t tenths;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false);
        void publish(DataModelTenthsInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
