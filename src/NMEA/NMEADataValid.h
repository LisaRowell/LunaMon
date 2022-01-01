#ifndef NMEA_DATA_VALID_H
#define NMEA_DATA_VALID_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEADataValid : public LoggableItem {
    private:
        bool valid;

    public:
        NMEADataValid();
        bool set(String &validStr);
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
