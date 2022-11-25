#ifndef MQTT_UNSUBSCRIBE_ACK_MESSAGE_H
#define MQTT_UNSUBSCRIBE_ACK_MESSAGE_H

#include "MQTTConnection.h"

#include <stdint.h>

struct MQTTUnsubscribeAckVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

bool sendMQTTUnsubscribeAckMessage(MQTTConnection *connection, uint16_t packetId);

#endif
