#ifndef NMEA_HUNDREDTHS_UINT8_H
#define NMEA_HUNDREDTHS_UINT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelHundredthsUInt8Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

#include <stdint.h>

class NMEAHundredthsUInt8 : public LoggableItem {
    private:
        uint8_t wholeNumber;
        uint8_t hundredths;

        bool set(const etl::string_view &decimalString);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
        void publish(DataModelHundredthsUInt8Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
