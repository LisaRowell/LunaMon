#include "MQTTSubscribeAckMessage.h"

#include <Arduino.h>

#include "MQTTUtil.h"

bool sendMQTTSubscribeAckMessage(MQTTConnection *connection, uint16_t packetId,
                                 uint8_t numberResults, uint8_t *results) {
    MQTTFixedHeader fixedHeader;
    MQTTSubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_SUBACK << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTSubscribeAckVariableHeader) + numberResults;
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (!connection->write((uint8_t *)&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    if (!connection->write(results, numberResults * sizeof(uint8_t))) {
        return false;
    }

    return true;
}

uint8_t mqttSubscribeResult(bool success, uint8_t maxQoS) {
    if (!success) {
        return MQTT_SUBACK_FAILURE_FLAG;
    } else {
        return maxQoS;
    }
}
