#ifndef NMEA_MESSAGE_HANDLER_H
#define NMEA_MESSAGE_HANDLER_H

#include "NMEAMessage.h"

class NMEAMessageHandler {
    public:
        virtual void processMessage(NMEAMessage *message) = 0;
};

#endif
