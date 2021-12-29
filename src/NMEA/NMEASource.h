#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

#include "NMEALine.h"

class NMEASource {
    private:
        Stream &stream;
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
        NMEASource(Stream &stream);
        void service();
};

#endif