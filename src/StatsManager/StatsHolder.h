#ifndef STATS_HOLDER_H
#define STATS_HOLDER_H

#include <stdint.h>

class StatsHolder {
    public:
        virtual void exportStats(uint32_t msElapsed) = 0;
};

#endif
