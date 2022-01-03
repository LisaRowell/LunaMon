#ifndef NMEA_HUNDREDTHS_UINT16_H
#define NMEA_HUNDREDTHS_UINT16_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEAHundredthsUInt16 : public LoggableItem {
    private:
        uint16_t wholeNumber;
        uint8_t hundredths;

    public:
        bool set(const String &decimalString);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
