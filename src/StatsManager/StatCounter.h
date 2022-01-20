#ifndef STAT_COUNTER_H
#define STAT_COUNTER_H

#include <Arduino.h>

#include "DataModel/DataModelLeaf.h"

class StatCounter {
    private:
        uint32_t count;
        uint32_t lastIntervalCount;

    public:
        StatCounter();
        StatCounter operator ++ (int);
        void update(DataModelLeaf &countLeaf, DataModelLeaf &rateLeaf, uint32_t msElapsed);
};

#endif
