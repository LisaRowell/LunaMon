#ifndef NMEA_LATITUDE_H
#define NMEA_LATITUDE_H

#include "NMEACoordinate.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

enum NorthOrSouth {
    NORTH,
    SOUTH
};

class NMEALatitude : public NMEACoordinate, public LoggableItem {
    private:
        enum NorthOrSouth northOrSouth;

        bool set(const etl::string_view &latitudeView, const etl::string_view &northOrSouthView);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
