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

#ifndef NMEA_DATA_MODEL_BRIDGE_H
#define NMEA_DATA_MODEL_BRIDGE_H

class NMEADBKMessage;
class NMEADBSMessage;
class NMEADBTMessage;
class NMEAGGAMessage;
class NMEAGLLMessage;
class NMEAGSAMessage;
class NMEAGSTMessage;
class NMEARMCMessage;
class NMEAVTGMessage;
class StatsMaanger;

#include "NMEA/NMEAMessageHandler.h"
#
#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsHolder.h"

#include <stdint.h>

class NMEADataModelBridge : public NMEAMessageHandler, public StatsHolder {
    private:
        StatCounter messagesBridgedCounter;

        void bridgeNMEADBKMessage(NMEADBKMessage *message);
        void bridgeNMEADBSMessage(NMEADBSMessage *message);
        void bridgeNMEADBTMessage(NMEADBTMessage *message);
        void bridgeNMEAGGAMessage(NMEAGGAMessage *message);
        void bridgeNMEAGLLMessage(NMEAGLLMessage *message);
        void bridgeNMEAGSAMessage(NMEAGSAMessage *message);
        void bridgeNMEAGSTMessage(NMEAGSTMessage *message);
        void bridgeNMEARMCMessage(NMEARMCMessage *message);
        void bridgeNMEAVTGMessage(NMEAVTGMessage *message);

    public:
        NMEADataModelBridge(StatsManager &statsManager);
        virtual void processMessage(NMEAMessage *message) override;
        virtual void exportStats(uint32_t msElapsed) override;
};

#endif
