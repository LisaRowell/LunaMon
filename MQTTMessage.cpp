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

// Returns false if there's a protocol error that requires dropping the connection flat out.
// A true return still needs to check the errorCode and if not CONNECTION_ACCEPTED,
// send an error CONNACK. The caller can also CONNACK with an error for it's own reasons.
bool MQTTMessage::parseConnectMessage(uint8_t &errorCode, char *clientID) {
  if (fixedHeaderFlags() != 0) {
    Serial.println("Received MQTT CONNECT message with non-zero Fixed Header Flags.");
    return false;
  }
  
  const uint32_t fixedHdrSize = fixedHeaderSize();
  const uint32_t bytesAfterFixedHdr = length - fixedHdrSize;

  if (bytesAfterFixedHdr < sizeof(struct MQTTConnectVariableHeader)) {
    Serial.println("Received MQTT CONNECT message with a size too small for the Variable Header.");
    return false;
  }

  uint8_t *variableHeaderStart = ((uint8_t *)fixedHeader) + fixedHdrSize;
  struct MQTTConnectVariableHeader *variableHdr = (struct MQTTConnectVariableHeader *)variableHeaderStart;
  variableHeader.connect = variableHdr;
  const uint32_t bytesAfterVariableHdr = bytesAfterFixedHdr - sizeof(MQTTConnectVariableHeader);

  const uint32_t protocolNameLength = variableHdr->protocolNameLengthMSB * 256 + variableHdr->protocolNameLengthLSB;
  if (protocolNameLength != 4 ||
      variableHdr->protocolName[0] != 'M' || variableHdr->protocolName[1] != 'Q' ||
      variableHdr->protocolName[2] != 'T' || variableHdr->protocolName[3] != 'T') {
    Serial.println("Received MQTT CONNECT message with non MQTT protocol string");
    return false;
  }

  if (variableHdr->level != MQTT_PROTOCOL_LEVEL) {
    Serial.print("Unsupported MQTT protocol level in CONNECT message. Expected ");
    Serial.print(MQTT_PROTOCOL_LEVEL);
    Serial.print(" got ");
    Serial.println(variableHdr->level);
    errorCode = MQTT_CONNACK_REFUSED_PROTOCOL_VERSION;
    return true;
  }

  uint8_t flags = variableHdr->flags;
  if ((flags & MQTT_CONNECT_FLAGS_RESERVED_MASK) != 0) {
    Serial.println("MQTT CONNECT message with non-zero Reserved flag in variable header");
    return false;
  }

  if ((flags & MQTT_CONNECT_FLAGS_CLEAN_SESSION_MASK) == 0) {
    Serial.println("MQTT CONNECT message with Clean Session not set");
    return false;
  }

  if ((flags & MQTT_CONNECT_FLAGS_WILL_MASK) !=0) {
    Serial.println("MQTT CONNECT message with Will set");
    return false;
  }

  if ((flags & MQTT_CONNECT_FLAGS_WILL_QOS_MASK) != 0) {
    Serial.println("MQTT CONNECT message with non-zero Will QoS");
    return false;
  }

  if ((flags & MQTT_CONNECT_FLAGS_WILL_RETAIN_MASK) != 0) {
    Serial.println("MQTT CONNECT message with Will Retain set");
    return false;
  }

  if ((flags & MQTT_CONNECT_FLAGS_PASSWORD_MASK) != 0) {
    Serial.println("MQTT CONNECT message with Password set");
    errorCode = MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD;
    return true;
  }

  if ((flags & MQTT_CONNECT_FLAGS_USER_NAME_MASK) != 0) {
    Serial.println("MQTT CONNECT message with Password set");
    errorCode = MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD;
    return true;
  }

  const uint16_t keepAliveInterval = variableHdr->keepAliveMSB * 256 + variableHdr->keepAliveLSB;
  Serial.print("MQTT CONNECT keepalive ");
  Serial.print(keepAliveInterval);
  Serial.println(" seconds.");

  if (bytesAfterVariableHdr < 2) {
    Serial.println("MQTT Connect packet with missing payload");
    return false;
  }

  uint8_t *payload = variableHeaderStart + sizeof(MQTTConnectVariableHeader);
  uint32_t clientIDLengthInMsg;
  if (!extractString(payload, bytesAfterVariableHdr, clientIDLengthInMsg, clientID, MQTT_MAX_CLIENT_ID_LENGTH)) {
    if (clientIDLengthInMsg > bytesAfterVariableHdr) {
      Serial.println("MQTT CONNECT packet with payload too small for it's Client ID");
      return false;
    } else {
      Serial.print("MQTT CONNECT packet with too long of an identifier (");
      Serial.print(clientIDLengthInMsg - 2);
      Serial.println(")");
      errorCode = MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED;
      return true;      
    }
  }

  Serial.print("MQTT Connect with Client ID = ");
  Serial.println(clientID);

  return true;
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