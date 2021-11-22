#ifndef NMEA_MESSAGE_HANDLER_H
#define NMEA_MESSAGE_HANDLER_H

#include "NMEALine.h"


class NMEAMessageHandler {
  public:
    void processLine(NMEALine &msgLine);
};

#endif