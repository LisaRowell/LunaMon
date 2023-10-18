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

#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

class NMEAMessageHandler;
class DataModelLeaf;
class StatsManager;

#include "NMEALine.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsHolder.h"

#include <etl/vector.h>

#include <Stream.h>

#include <stddef.h>

class NMEASource : public StatsHolder {
    private:
        Stream &stream;
        char buffer[maxNMEALineLength];
        size_t bufferPos;
        size_t remaining;
        bool carriageReturnFound;
        NMEALine inputLine;
        static const size_t maxMessageHandlers = 5;
        etl::vector<NMEAMessageHandler *, maxMessageHandlers> messageHandlers;
        StatCounter messagesCounter;
        DataModelLeaf &messageCountDataModelLeaf;
        DataModelLeaf &messageRateDataModelLeaf;

        bool scanForCarriageReturn(size_t &carriageReturnPos);
        bool readAvailableInput();
        bool processBuffer();
        void lineCompleted();
        void updateStats();

    public:
        NMEASource(Stream &stream, DataModelLeaf &messageCountDataModelLeaf,
                   DataModelLeaf &messageRateDataModelLeaf, StatsManager &statsManager);
        void addMessageHandler(NMEAMessageHandler &messageHandler);
        void service();
        virtual void exportStats(uint32_t msElapsed) override;
};

#endif
