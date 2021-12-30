#ifndef TIME_H
#define TIME_H

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEATime : public LoggableItem {
    private:
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t secondPrecision;
        uint32_t secondFraction;

        void buildSecondsFactionString(char *string) const;

    public:
        bool set(const String &timeStr);
        void publish(DataModelLeaf &leaf);
        virtual void log(Logger &logger) const override;
};

#endif
