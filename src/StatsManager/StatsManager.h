#ifndef STATS_MANAGER_H
#define STATS_MANAGER_H

#include <Arduino.h>
#include <Array.h>

#include "StatsHolder.h"

#include "Util/PassiveTimer.h"

class StatsManager {
    private:
        const uint32_t statsUpdateTimeInterval = 10;
        PassiveTimer statsUpdateTimer;
        PassiveTimer lastHarvestTime;
        Array<StatsHolder *, 10> statsHolders;

    public:
        StatsManager();
        void addStatsHolder(StatsHolder *statsHolder);
        void removeStatsHolder(StatsHolder *statsHolder);
        void service();
};

#endif
