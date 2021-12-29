#ifndef NMEA_LONGITUDE_H
#define NMEA_LONGITUDE_H

#include "NMEACoordinate.h"

enum EastOrWest {
    EAST,
    WEST
};

class NMEALongitude : public NMEACoordinate {
    private:
        enum EastOrWest eastOrWest;

    public:
        bool set(const String &string, const String &eastOrWestStr);
        void print();
        void publish(DataModelLeaf &leaf);
};

#endif