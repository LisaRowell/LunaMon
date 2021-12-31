#ifndef NMEA_DATA_MODEL_BRIDGE_H
#define NMEA_DATA_MODEL_BRIDGE_H

#include "NMEA/NMEAMessageHandler.h"
#include "NMEA/NMEAGLLMessage.h"

class NMEADataModelBridge : public NMEAMessageHandler {
    private:
        void bridgeNMEAGLLMessage(NMEAGLLMessage *message);

    public:
        virtual void processMessage(NMEAMessage *message) override;
};

#endif