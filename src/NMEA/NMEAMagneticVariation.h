#ifndef NMEA_MAGNETIC_VARIATION_H
#define NMEA_MAGNETIC_VARIATION_H

#include "NMEA/NMEALine.h"
#include "NMEA/NMEATalker.h"

#include "DataModel/DataModelTenthsInt16Leaf.h"

#include "Util/Logger.h"
#include "Util/LoggableItem.h"

#include <Arduino.h>

class NMEAMagneticVariation : public LoggableItem {
    private:
        int16_t direction;
        uint8_t tenths;
        bool hasValue;

    public:
        bool set(const String &directionStr, const String &eastOrWestStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelTenthsInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
