#include "MQTTPingResponseMessage.h"
#include "MQTTUtil.h"

#include <Arduino.h>

bool sendMQTTPingResponseMessage(MQTTConnection *connection) {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PINGRESP << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }

    if (!mqttWriteRemainingLength(connection, 0)) {
        return false;
    }

    return true;
}
