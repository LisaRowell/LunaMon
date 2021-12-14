#ifndef MQTT_PING_RESPONSE_MESSAGE_H
#define MQTT_PING_RESPONSE_MESSAGE_H

#include <Arduino.h>

#include "MQTTConnection.h"

bool sendMQTTPingResponseMessage(MQTTConnection *connection);

#endif
