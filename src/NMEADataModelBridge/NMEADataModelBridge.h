#ifndef NMEA_DATA_MODEL_BRIDGE_H
#define NMEA_DATA_MODEL_BRIDGE_H

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
