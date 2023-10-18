/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "StatsManager.h"
#include "StatsHolder.h"

#include "Util/PassiveTimer.h"
#include "Util/TimeConstants.h"
#include "Util/Logger.h"
#include "Util/Error.h"

#include <etl/vector.h>

#include <stdint.h>

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
    for (auto iterator = statsHolders.begin(); iterator != statsHolders.end(); iterator++) {
        if (*iterator == statsHolder) {
            statsHolders.erase(iterator);
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
