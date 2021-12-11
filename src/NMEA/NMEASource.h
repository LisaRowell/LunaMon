#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

#include "NMEALine.h"
#include "NMEAMessageHandler.h"

class NMEASource {
    private:
        Stream &stream;
        NMEAMessageHandler &messageHandler;
        char buffer[maxNMEALineLength];
        unsigned bufferPos;
        unsigned remaining;
        bool carriageReturnFound;
        NMEALine inputLine;

        bool scanForCarriageReturn(unsigned &carriageReturnPos);
        bool readAvailableInput();
        bool processBuffer();
        void lineCompleted();

    public:
        NMEASource(Stream &stream, NMEAMessageHandler &messageHandler);
        void service();
};

#endif