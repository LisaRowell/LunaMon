#ifndef MQTT_PUBLISH_MESSAGE_H
#define MQTT_PUBLISH_MESSAGE_H

#include "MQTTConnection.h"

#include <stdint.h>

#define MQTT_PUBLISH_FLAGS_DUP_MASK    0x08
#define MQTT_PUBLISH_FLAGS_QOS_MASK    0x06
#define MQTT_PUBLISH_FLAGS_QOS_SHIFT 1
#define MQTT_PUBLISH_FLAGS_RETAIN_MASK 0x01

bool sendMQTTPublishMessage(MQTTConnection *connection, const char *topic, const char *value,
                            bool dup, uint8_t qosLevel, bool retain,
                            uint16_t packetId);

#endif
