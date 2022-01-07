#ifndef NMEA_DATA_MODEL_BRIDGE_H
#define NMEA_DATA_MODEL_BRIDGE_H

#include "NMEA/NMEAMessageHandler.h"
#include "NMEA/NMEAGGAMessage.h"
#include "NMEA/NMEAGLLMessage.h"
#include "NMEA/NMEAGSAMessage.h"
#include "NMEA/NMEAGSTMessage.h"
#include "NMEA/NMEARMCMessage.h"
#include "NMEA/NMEAVTGMessage.h"

class NMEADataModelBridge : public NMEAMessageHandler {
    private:
        void bridgeNMEAGGAMessage(NMEAGGAMessage *message);
        void bridgeNMEAGLLMessage(NMEAGLLMessage *message);
        void bridgeNMEAGSAMessage(NMEAGSAMessage *message);
        void bridgeNMEAGSTMessage(NMEAGSTMessage *message);
        void bridgeNMEARMCMessage(NMEARMCMessage *message);
        void bridgeNMEAVTGMessage(NMEAVTGMessage *message);

    public:
        virtual void processMessage(NMEAMessage *message) override;
};

#endif
