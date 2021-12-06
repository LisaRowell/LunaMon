#include <Arduino.h>

#include "MQTTConnectMessage.h"
#include "MQTTMessage.h"
#include "MQTTConnectAckMessage.h"

MQTTConnectMessage::MQTTConnectMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

// Returns false if there's a protocol error that requires dropping the connection flat out.
bool MQTTConnectMessage::parse() {
  if (fixedHeaderFlags() != 0) {
    Serial.println("Received MQTT CONNECT message with non-zero Fixed Header Flags.");
    return false;
  }
  
  if (bytesAfterFixedHdr < sizeof(MQTTConnectVariableHeader)) {
    Serial.println("Received MQTT CONNECT message with a size too small for the Variable Header.");
    return false;
  }

  variableHeader = (MQTTConnectVariableHeader *)variableHeaderStart;
  const uint32_t bytesAfterVariableHdr = bytesAfterFixedHdr - sizeof(MQTTConnectVariableHeader);

  const uint32_t protocolNameLength = variableHeader->protocolNameLengthMSB * 256 + variableHeader->protocolNameLengthLSB;
  if (protocolNameLength != 4 ||
      variableHeader->protocolName[0] != 'M' || variableHeader->protocolName[1] != 'Q' ||
      variableHeader->protocolName[2] != 'T' || variableHeader->protocolName[3] != 'T') {
    Serial.println("Received MQTT CONNECT message with non MQTT protocol string");
    return false;
  }

  uint8_t flags = variableHeader->flags;
  if ((flags & MQTT_CONNECT_FLAGS_RESERVED_MASK) != 0) {
    Serial.println("MQTT CONNECT message with non-zero Reserved flag in variable header");
    return false;
  }

  if (hasWill()) {
    const uint8_t willQoS = MQTTConnectMessage::willQoS();
    if (willQoS > 2) {
      Serial.print("MQTT CONNECT message with bad Will QoS (");
      Serial.print(willQoS);
      Serial.println(")");
      return false;
    }
  } else {
    if (willQoS() != 0) {
      Serial.println("MQTT CONNECT message with Will QoS, but no Will");
      return false;
    }
    if (willRetain()) {
      Serial.println("MQTT CONNECT message with Will Retain, but no Will");
      return false;
    }
  }

  uint8_t *payloadPos = variableHeaderStart + sizeof(MQTTConnectVariableHeader);
  uint32_t payloadBytesRemaining = bytesAfterVariableHdr;

  if (!parseString(clientIDStr, payloadPos, payloadBytesRemaining)) {
    Serial.println("MQTT CONNECT packet with payload too small for its Client ID");
    return false;
  }

  if (hasWill()) {
    if (!parseString(willTopicStr, payloadPos, payloadBytesRemaining)) {
      Serial.println("MQTT CONNECT packet with payload too small for its Will Topic");
      return false;
    }
    if (!parseString(willMessageStr, payloadPos, payloadBytesRemaining)) {
      Serial.println("MQTT CONNECT packet with payload too small for its Will Message");
      return false;
    }
  }

  if (hasUserName()) {
    if (!parseString(userNameStr, payloadPos, payloadBytesRemaining)) {
      Serial.println("MQTT CONNECT packet with payload too small for its User Name");
      return false;
    }
  }

  if (hasPassword()) {
    if (!parseString(passwordStr, payloadPos, payloadBytesRemaining)) {
      Serial.println("MQTT CONNECT packet with payload too small for its Password");
      return false;
    }
  }

  if (payloadBytesRemaining) {
    Serial.print("MQTT CONNECT packet with ");
    Serial.print(payloadBytesRemaining);
    Serial.println(" extra bytes");
    return false;
  }

  return true;
}

uint8_t MQTTConnectMessage::sanityCheck() {
  if (variableHeader->level != MQTT_PROTOCOL_LEVEL) {
    Serial.print("Unsupported MQTT protocol level in CONNECT message. Expected ");
    Serial.print(MQTT_PROTOCOL_LEVEL);
    Serial.print(" got ");
    Serial.println(variableHeader->level);
    return MQTT_CONNACK_REFUSED_PROTOCOL_VERSION;
  }

  // We accept zero length client IDs, but it's an error for the client
  // to send one if it's not also requesting a clean session since a
  // broker couldn't pair them up in that case.
  if (clientIDStr->length() == 0 && !cleanSession()) {
    Serial.println("MQTT CONNECT message with a zero length Client ID and Clean Session false");
    return MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED;
  }

  Serial.print("MQTT Connect with Client ID = ");
  clientIDStr->print();
  Serial.println();

  return MQTT_CONNACK_ACCEPTED;
}

bool MQTTConnectMessage::cleanSession() {
  return (variableHeader->flags & MQTT_CONNECT_FLAGS_CLEAN_SESSION_MASK) != 0;
}

bool MQTTConnectMessage::hasWill() {
  return (variableHeader->flags & MQTT_CONNECT_FLAGS_WILL_MASK) !=0;
}

uint8_t MQTTConnectMessage::willQoS() {
  return (variableHeader->flags & MQTT_CONNECT_FLAGS_WILL_QOS_MASK) >> MQTT_CONNECT_FLAGS_WILL_QOS_SHIFT;
}

bool MQTTConnectMessage::willRetain() {
  return (variableHeader->flags & MQTT_CONNECT_FLAGS_WILL_RETAIN_MASK) != 0;
}

bool MQTTConnectMessage::hasUserName() {
  return (variableHeader->flags & MQTT_CONNECT_FLAGS_USER_NAME_MASK) != 0;
}

bool MQTTConnectMessage::hasPassword() {
  return (variableHeader->flags & MQTT_CONNECT_FLAGS_PASSWORD_MASK) != 0;
}

uint16_t MQTTConnectMessage::keepAliveSec() {
  return variableHeader->keepAliveMSB * 256 + variableHeader->keepAliveLSB;
}

const MQTTString *MQTTConnectMessage::clientID() {
  return clientIDStr;
}
