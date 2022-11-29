#ifndef NMEA_LONGITUDE_H
#define NMEA_LONGITUDE_H

#include "NMEACoordinate.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

enum EastOrWest {
    EAST,
    WEST
};

class NMEALongitude : public NMEACoordinate, public LoggableItem {
    private:
        enum EastOrWest eastOrWest;

        bool set(const etl::string_view &longitudeView, const etl::string_view &eastOrWestView);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
