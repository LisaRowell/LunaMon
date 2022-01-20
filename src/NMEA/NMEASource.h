#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessageHandler.h"

#include "DataModel/DataModelLeaf.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsHolder.h"
#include "StatsManager/StatsManager.h"

#include "Util/PassiveTimer.h"

class NMEASource : public StatsHolder {
    private:
        Stream &stream;
        char buffer[maxNMEALineLength];
        unsigned bufferPos;
        unsigned remaining;
        bool carriageReturnFound;
        NMEALine inputLine;
        static const unsigned maxMessageHandlers = 5;
        NMEAMessageHandler *messageHandlers[maxMessageHandlers];
        unsigned numberMessageHandlers;
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
