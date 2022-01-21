#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

#include "NMEALine.h"
#include "NMEAMessageHandler.h"

#include "DataModel/DataModelLeaf.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsHolder.h"
#include "StatsManager/StatsManager.h"

#include "Util/PassiveTimer.h"

#include <Arduino.h>
#include <Array.h>

class NMEASource : public StatsHolder {
    private:
        Stream &stream;
        char buffer[maxNMEALineLength];
        unsigned bufferPos;
        unsigned remaining;
        bool carriageReturnFound;
        NMEALine inputLine;
        static const size_t maxMessageHandlers = 5;
        Array<NMEAMessageHandler *, maxMessageHandlers> messageHandlers;
        StatCounter messagesCounter;
        DataModelLeaf &messageCountDataModelLeaf;
        DataModelLeaf &messageRateDataModelLeaf;

        bool scanForCarriageReturn(unsigned &carriageReturnPos);
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
