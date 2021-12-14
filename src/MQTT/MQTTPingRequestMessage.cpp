#include <Arduino.h>

#include "MQTTPingRequestMessage.h"
#include "Util/Logger.h"

MQTTPingRequestMessage::MQTTPingRequestMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

bool MQTTPingRequestMessage::parse() {
    if (fixedHeaderFlags() != 0x0) {
        logger << logWarning << "Received MQTT PINGREQ message with invalid Fixed Header Flags"
               << eol;
        return false;
    }

    if (bytesAfterFixedHdr > 0) {
        logger << logWarning << "Received MQTT PINGREQ message with extra data after Fixed Header"
               << eol;
        return false;
    }

    return true;
}
