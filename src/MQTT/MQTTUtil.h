#ifndef MQTT_UTIL_H
#define MQTT_UTIL_H

#include "MQTTConnection.h"

extern bool mqttWriteRemainingLength(MQTTConnection *connection, uint32_t remainingLength);

#endif