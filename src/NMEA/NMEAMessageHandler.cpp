#include <Arduino.h>

#include "NMEAMessageHandler.h"
#include "NMEALine.h"
#include "NMEAMessage.h"

void NMEAMessageHandler::processLine(NMEALine &msgLine) {
    NMEAMessage *nmeaMessage = parseNMEAMessage(msgLine);
    if (nmeaMessage != NULL) {
        nmeaMessage->print();
        // While we're done with the nmeaMessage, we don't do a free here since it was allocated
        // with a static buffer and placement new.
  }
}