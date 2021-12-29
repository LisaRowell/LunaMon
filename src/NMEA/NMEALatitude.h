#ifndef NMEA_LATITUDE_H
#define NMEA_LATITUDE_H

#include "NMEACoordinate.h"

#include "DataModel/DataModelLeaf.h"

enum NorthOrSouth {
    NORTH,
    SOUTH
};

class NMEALatitude : public NMEACoordinate {
    private:
        enum NorthOrSouth northOrSouth;

    public:
        bool set(const String &string, const String &northOrSouthStr);
        void print();
        void publish(DataModelLeaf &leaf);
};

#endif
