#include "StatsManager.h"
#include "StatsHolder.h"

#include "Util/PassiveTimer.h"
#include "Util/TimeConstants.h"
#include "Util/Logger.h"
#include "Util/Error.h"

#include <Arduino.h>
#include <Array.h>

StatsManager::StatsManager() : statsHolders() {
    lastHarvestTime.setNow();
    statsUpdateTimer.setSeconds(statsUpdateTimeInterval);
}

void StatsManager::addStatsHolder(StatsHolder *statsHolder) {
    if (statsHolders.full()) {
        fatalError("Attempt to add more than the maximum number of stats holders");
    }

    statsHolders.push_back(statsHolder);
}

void StatsManager::removeStatsHolder(StatsHolder *statsHolder) {
    for (unsigned statsHolderIndex = 0;
         statsHolderIndex < statsHolders.size();
         statsHolderIndex++) {
        if (statsHolders[statsHolderIndex] == statsHolder) {
            statsHolders.remove(statsHolderIndex);
            return;
        }
    }

    fatalError("Failed to find stats holder while trying to remove it");
}

void StatsManager::service() {
    if (statsUpdateTimer.expired()) {
        // This should changed to be a based of the actual time passed
        const uint32_t elapsedTime = lastHarvestTime.elapsedTime();
        lastHarvestTime.setNow();

        logger << logDebugStatsManager << "Harvesting stats with elapsed time " << elapsedTime
               << "ms" << eol;

        for (StatsHolder *statsHolder : statsHolders) {
            statsHolder->exportStats(elapsedTime);
        }

        statsUpdateTimer.advanceSeconds(statsUpdateTimeInterval);
    }
}
