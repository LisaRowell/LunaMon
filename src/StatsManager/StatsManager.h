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
