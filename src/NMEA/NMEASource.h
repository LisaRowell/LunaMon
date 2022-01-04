#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessageHandler.h"

class NMEASource {
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

        bool scanForCarriageReturn(unsigned &carriageReturnPos);
        bool readAvailableInput();
        bool processBuffer();
        void lineCompleted();

    public:
        NMEASource(Stream &stream);
        void addMessageHandler(NMEAMessageHandler &messageHandler);
        void service();
};

#endif
