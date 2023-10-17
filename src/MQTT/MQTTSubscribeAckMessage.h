#ifndef MQTT_SUBSCRIBE_ACK_MESSAGE_H
#define MQTT_SUBSCRIBE_ACK_MESSAGE_H

#include <stdint.h>

struct MQTTSubscribeAckVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

#define MQTT_SUBACK_FAILURE_FLAG 0x80

#endif
