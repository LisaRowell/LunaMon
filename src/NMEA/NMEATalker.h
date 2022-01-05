#ifndef NMEA_TALKER_H
#define NMEA_TALKER_H

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEATalker : public LoggableItem {
    private:
        char talkerCode[2];

    public:
        NMEATalker(String &talkerCode);
        const char *name() const;
        virtual void log(Logger &logger) const override;
};

#endif
