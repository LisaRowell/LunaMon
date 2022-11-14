#ifndef NMEA_HUNDREDTHS_UINT16_H
#define NMEA_HUNDREDTHS_UINT16_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelHundredthsUInt16Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <Arduino.h>

class NMEAHundredthsUInt16 : public LoggableItem {
    private:
        uint16_t wholeNumber;
        uint8_t hundredths;

    public:
        bool set(const String &decimalString);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
        void publish(DataModelHundredthsUInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
