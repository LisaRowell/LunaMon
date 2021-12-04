#include <Arduino.h>

#include "MQTTDisconnectMessage.h"
#include "MQTTMessage.h"

MQTTDisconnectMessage::MQTTDisconnectMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

// Returns false if there's a protocol error that requires dropping the connection flat out.
// We do this anyway, despite a correct disconnect message yielding the same result so that we
// get log messages for malformed packets. 
bool MQTTDisconnectMessage::parse() {
  if (fixedHeaderFlags() != 0) {
    Serial.println("Received MQTT CONNECT message with non-zero Fixed Header Flags.");
    return false;
  }

  if (bytesAfterFixedHdr) {
    Serial.print("MQTT DISCONNECT packet with ");
    Serial.print(bytesAfterFixedHdr);
    Serial.println(" extra bytes");
    return false;
  }

  return true;
}