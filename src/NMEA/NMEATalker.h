#ifndef NMEA_TALKER_H
#define NMEA_TALKER_H

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEATalker : public LoggableItem {
    private:
        enum Talker {
        NMEA_TALKER_UNKNOWN,
        NMEA_TALKER_GPS
    };

    enum Talker talker;

    public:
        NMEATalker(String &talkerCode);
        const char *name() const;
        virtual void log(Logger &logger) const override;
};

#endif
