#ifndef NMEA_DATA_VALID_H
#define NMEA_DATA_VALID_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelUInt32Leaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

class NMEADataValid : public LoggableItem {
    private:
        bool valid;

        bool set(etl::string_view &dataValidView);

    public:
        NMEADataValid();
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelUInt32Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
