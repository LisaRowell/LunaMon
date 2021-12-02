#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

struct MQTTFixedHeader {
  uint8_t typeAndFlags;
  uint8_t remainingLength[];
};

#define MQTT_MSG_TYPE_MASK 0xf0
#define MQTT_MSG_TYPE_SHIFT 4
#define MQTT_MSG_FLAGS_MASK 0x0f
#define MQTT_MSG_FLAGS_SHIFT 4

enum MQTTMessageType : char {
  MQTT_MSG_RESERVED1 = 0,
  MQTT_MSG_CONNECT = 1,
  MQTT_MSG_CONNACK = 2,
  MQTT_MSG_PUBLISH = 3,
  MQTT_MSG_PUBACK = 4,
  MQTT_MSG_PUBREC = 5,
  MQTT_MSG_PUBREL = 6,
  MQTT_MSG_PUBCOMP = 7,
  MQTT_MSG_SUBSCRIBE = 8,
  MQTT_MSG_SUBACK = 9,
  MQTT_MSG_UNSUBSCRIBE = 10,
  MQTT_MSG_UNSUBACK = 11,
  MQTT_MSG_PINGREQ = 12,
  MQTT_MSG_PINGRESP = 13,
  MQTT_MSG_DISCONNECT = 14,
  MQTT_MSG_RESERVED2 = 15
};

struct MQTTConnectVariableHeader {
  uint8_t protocolNameLengthMSB;
  uint8_t protocolNameLengthLSB;
  uint8_t protocolName[4];
  uint8_t level;
  uint8_t flags;
  uint8_t keepAliveMSB;
  uint8_t keepAliveLSB;
};

#define MQTT_PROTOCOL_LEVEL 4

#define MQTT_CONNECT_FLAGS_RESERVED_MASK      0x01
#define MQTT_CONNECT_FLAGS_CLEAN_SESSION_MASK 0x02
#define MQTT_CONNECT_FLAGS_WILL_MASK          0x04
#define MQTT_CONNECT_FLAGS_WILL_QOS_MASK      0x18
#define MQTT_CONNECT_FLAGS_WILL_QOS_SHIFT 3
#define MQTT_CONNECT_FLAGS_WILL_RETAIN_MASK   0x20
#define MQTT_CONNECT_FLAGS_PASSWORD_MASK      0x40
#define MQTT_CONNECT_FLAGS_USER_NAME_MASK     0x80

#define MQTT_MAX_CLIENT_ID_LENGTH 23

#define MQTT_CONNACK_ACCEPTED                     0x00
#define MQTT_CONNACK_REFUSED_PROTOCOL_VERSION     0x01
#define MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED  0x02
#define MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE   0x03
#define MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD 0x04
#define MQTT_CONNACK_REFUSED_NOT_AUTHORIZED       0x05

class MQTTMessage {
  private:
    struct MQTTFixedHeader *fixedHeader;
    uint32_t length;
    union variableHeader_tag {
      struct MQTTConnectVariableHeader *connect;
      unsigned none;
    } variableHeader;

    uint8_t fixedHeaderFlags();
    uint32_t fixedHeaderSize();
    bool extractString(uint8_t *messagePos, uint32_t bytesLeftInMessage,
                       uint32_t &lengthInMessage, char *string, unsigned maxLength);

  public:
    MQTTMessage();
    MQTTMessage(uint8_t *messageData, uint32_t messageLength);
    enum MQTTMessageType messageType();
    const char *messageTypeStr();
    bool parseConnectMessage(uint8_t &errorCode, char *clientID);
};

#endif