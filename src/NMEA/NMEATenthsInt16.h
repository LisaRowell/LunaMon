#ifndef NMEA_TENTHS_INT16_H
#define NMEA_TENTHS_INT16_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEATenthsInt16 : public LoggableItem {
    private:
        int16_t integer;
        uint8_t tenths;

    public:
        bool set(const String &decimalString);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif