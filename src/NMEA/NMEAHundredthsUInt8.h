#ifndef NMEA_HUNDREDTHS_UINT8_H
#define NMEA_HUNDREDTHS_UINT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEAHundredthsUInt8 : public LoggableItem {
    private:
        uint8_t wholeNumber;
        uint8_t hundredths;

    public:
        bool set(const String &decimalString);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
