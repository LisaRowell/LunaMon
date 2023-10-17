#ifndef MQTT_UNSUBSCRIBE_ACK_MESSAGE_H
#define MQTT_UNSUBSCRIBE_ACK_MESSAGE_H

#include <stdint.h>

struct MQTTUnsubscribeAckVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

#endif
