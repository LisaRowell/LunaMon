#ifndef TIME_H
#define TIME_H

#include "DataModel/DataModelLeaf.h"

class Time {
    private:
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        uint8_t secondPrecision;
        uint32_t secondFraction;

        void buildSecondsFactionString(char *string);

    public:
        bool set(const String &timeStr);
        void print();
        void publish(DataModelLeaf &leaf);
};

#endif