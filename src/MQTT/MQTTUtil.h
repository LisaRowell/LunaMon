#ifndef MQTT_UTIL_H
#define MQTT_UTIL_H

#include "MQTTConnection.h"

#include <Arduino.h>

bool mqttWriteRemainingLength(MQTTConnection *connection, uint32_t remainingLength);
bool mqttWriteUInt16(MQTTConnection *connection, uint16_t value);
bool mqttWriteMQTTString(MQTTConnection *connection, const char *string);

#endif
