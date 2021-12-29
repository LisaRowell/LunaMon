#include <Arduino.h>

#include "NMEAMessageHandler.h"
#include "NMEALine.h"
#include "NMEAMessage.h"

void NMEAMessageHandler::processLine(NMEALine &msgLine) {
    parseNMEAMessage(msgLine);
}