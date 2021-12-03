#include <Arduino.h>

#include "MQTTMessage.h"
#include "Error.h"

MQTTMessage::MQTTMessage() : fixedHeader(NULL), length(0) {
}

MQTTMessage::MQTTMessage(uint8_t *messageData, uint32_t messageLength) :
                         fixedHeader((struct MQTTFixedHeader *)messageData),
                         length(messageLength) {

}

MQTTMessageType MQTTMessage::messageType() {
  uint8_t typeValue = (MQTTMessageType)(fixedHeader->typeAndFlags & MQTT_MSG_TYPE_MASK) >> MQTT_MSG_TYPE_SHIFT;
  return (MQTTMessageType)typeValue;
}

const char *MQTTMessage::messageTypeStr() {
  switch (messageType()) {
    case  MQTT_MSG_RESERVED1:
      return "Reserved (0)";
    case MQTT_MSG_CONNECT:
      return "CONNECT";
    case MQTT_MSG_CONNACK:
      return "CONNACK";
    case MQTT_MSG_PUBLISH:
      return "PUBLISH";
    case MQTT_MSG_PUBACK:
      return "PUBACK";
    case MQTT_MSG_PUBREC:
      return "PUBREC";
    case MQTT_MSG_PUBREL:
      return "PUBREL";
    case MQTT_MSG_PUBCOMP:
      return "PUBCOMP";
    case MQTT_MSG_SUBSCRIBE:
      return "SUBSCRIBE";
    case MQTT_MSG_SUBACK:
      return "SUBACK";
    case MQTT_MSG_UNSUBSCRIBE:
      return "SUBSCRIBE";
    case MQTT_MSG_UNSUBACK:
      return "UNSUBACK";
    case MQTT_MSG_PINGREQ:
      return "PINGEQ";
    case MQTT_MSG_PINGRESP:
      return "PINGRESP";
    case MQTT_MSG_DISCONNECT:
      return "DISCONNECT";
    case MQTT_MSG_RESERVED2:
      return "RESERVED (15)";
    default:
      fatalError("Wonkie MQTT message type enum");
  }
}

uint8_t MQTTMessage::fixedHeaderFlags() {
  return (fixedHeader->typeAndFlags & MQTT_MSG_FLAGS_MASK) >> MQTT_MSG_TYPE_SHIFT;
}

uint32_t MQTTMessage::fixedHeaderSize() {
  if ((fixedHeader->remainingLength[0] & 0x80) == 0) {
    return sizeof(MQTTFixedHeader) + 1;
  } else if ((fixedHeader->remainingLength[1] & 0x80) == 0) {
    return sizeof(MQTTFixedHeader) + 2;
  } else if ((fixedHeader->remainingLength[2] & 0x80) == 0) {
    return sizeof(MQTTFixedHeader) + 3;
  } else {
    return sizeof(MQTTFixedHeader) + 4;
  }
}

// Returns false if the string is too long for the given string buffer.
// messagePos is advanced so as to point to the byte after the string.
bool MQTTMessage::extractString(uint8_t *messagePos, uint32_t bytesLeftInMessage,
                                uint32_t &lengthInMessage, char *string, unsigned maxLength) {
  if (bytesLeftInMessage < 2) {
    // Messed up message with out enough bytes for length.
    lengthInMessage = 2;
    return false;
  }
  uint16_t stringLength = *messagePos * 256 + *(messagePos + 1);
  messagePos += 2;
  lengthInMessage = (uint32_t)stringLength + 2;

  if (lengthInMessage > bytesLeftInMessage) {
    // Again, not enough bytes in the message for the string
    return false;
  }

  if (stringLength > maxLength) {
    // String is longer than we accept
    return false;
  }

  for (; stringLength; stringLength--, messagePos++, string++) {
    *string = *messagePos;
  }
  *string = 0;

  return true;
}