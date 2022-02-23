#ifndef STAT_COUNTER_H
#define STAT_COUNTER_H

class DataModelLeaf;

#include <stdint.h>

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
