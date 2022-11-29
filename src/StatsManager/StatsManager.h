#ifndef STATS_MANAGER_H
#define STATS_MANAGER_H

#include "StatsHolder.h"

#include "Util/PassiveTimer.h"

#include <etl/vector.h>
#include <stdint.h>

class StatsManager {
    private:
        const uint32_t statsUpdateTimeInterval = 10;
        PassiveTimer statsUpdateTimer;
        PassiveTimer lastHarvestTime;
        etl::vector<StatsHolder *, 10> statsHolders;

    public:
        StatsManager();
        void addStatsHolder(StatsHolder *statsHolder);
        void removeStatsHolder(StatsHolder *statsHolder);
        void service();
};

#endif
