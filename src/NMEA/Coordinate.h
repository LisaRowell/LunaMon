#ifndef COORDINATE_H
#define COORDINATE_H

#include "DataModel/DataModelLeaf.h"

class Coordinate {
    protected:
        uint8_t degrees;
        float minutes;

        bool setDegrees(const String &string, unsigned startDegrees, unsigned endDegrees,
                        uint8_t maxDegrees);
        bool setMinutes(const String &string, unsigned startMinutes);
        void print();
        void publish(DataModelLeaf &leaf, bool isPositive);
};

#endif