#ifndef LONGITUDE_H
#define LONGITUDE_H

#include "Coordinate.h"

enum EastOrWest {
    EAST,
    WEST
};

class Longitude : public Coordinate {
    private:
        enum EastOrWest eastOrWest;

    public:
        bool set(const String &string, const String &eastOrWestStr);
        void print();
        void publish(DataModelLeaf &leaf);
};

#endif