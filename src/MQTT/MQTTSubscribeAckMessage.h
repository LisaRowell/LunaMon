#ifndef MQTT_SUBSCRIBE_ACK_MESSAGE_H
#define MQTT_SUBSCRIBE_ACK_MESSAGE_H

#include "MQTTConnection.h"

struct MQTTSubscribeAckVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

#define MQTT_SUBACK_FAILURE_FLAG 0x80

bool sendMQTTSubscribeAckMessage(MQTTConnection *connection, uint16_t packetId,
                                 uint8_t numberResults, uint8_t *results);
uint8_t mqttSubscribeResult(bool success, uint8_t maxQoS);

#endif