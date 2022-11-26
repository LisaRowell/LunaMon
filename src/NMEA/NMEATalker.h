#ifndef NMEA_TALKER_H
#define NMEA_TALKER_H

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string.h>
#include <Arduino.h>

class NMEATalker : public LoggableItem {
    private:
        etl::string<2> talkerCode;

    public:
        NMEATalker(const etl::istring &talkerCode);
        const char *name() const;
        virtual void log(Logger &logger) const override;
};

#endif
