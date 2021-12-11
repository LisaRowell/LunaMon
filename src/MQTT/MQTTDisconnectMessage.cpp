#include <Arduino.h>

#include "MQTTDisconnectMessage.h"
#include "MQTTMessage.h"
#include "Util/Logger.h"

MQTTDisconnectMessage::MQTTDisconnectMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

// Returns false if there's a protocol error that requires dropping the connection flat out. We do
// this anyway, despite a correct disconnect message yielding the same result so that we get log
// messages for malformed packets. 
bool MQTTDisconnectMessage::parse() {
    if (fixedHeaderFlags() != 0) {
        logger << logWarning << "Received MQTT CONNECT message with non-zero Fixed Header Flags."
               << eol;
        return false;
    }

    if (bytesAfterFixedHdr) {
        logger << logWarning << "MQTT DISCONNECT packet with " << bytesAfterFixedHdr
               << " extra bytes" << eol;
        return false;
    }

    return true;
}