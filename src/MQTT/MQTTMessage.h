#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

#include "MQTTString.h"

struct MQTTFixedHeader {
    uint8_t typeAndFlags;
    uint8_t remainingLength[];
};

#define MQTT_MSG_TYPE_MASK 0xf0
#define MQTT_MSG_TYPE_SHIFT 4
#define MQTT_MSG_FLAGS_MASK 0x0f
#define MQTT_MSG_FLAGS_SHIFT 0

enum MQTTMessageType : uint8_t {
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

class MQTTMessage {
    protected:
        struct MQTTFixedHeader *fixedHeader;
        uint32_t length;
        uint32_t fixedHdrSize;
        uint32_t bytesAfterFixedHdr;
        uint8_t *variableHeaderStart;

        uint8_t fixedHeaderFlags() const;
        uint32_t fixedHeaderSize() const;
        bool parseString(MQTTString * &string, uint8_t *messagePos, uint32_t &bytesRemaining);

    public:
        MQTTMessage();
        MQTTMessage(uint8_t *messageData, uint32_t messageLength);
        enum MQTTMessageType messageType() const;
        const char *messageTypeStr() const;
};

#endif