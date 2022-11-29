#ifndef NMEA_DATE_H
#define NMEA_DATE_H

#include "NMEA/NMEALine.h"
#include "NMEA/NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"
#include "Util/LoggableItem.h"

#include <etl/string_view.h>

#include <stddef.h>

class NMEADate : public LoggableItem {
    private:
        bool hasValue;
        uint16_t year;
        uint8_t month;
        uint8_t day;

        bool set(const etl::string_view &dateView);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
