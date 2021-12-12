#include "MQTTConnectAckMessage.h"

#include <Arduino.h>

#include "MQTTConnection.h"
#include "MQTTMessage.h"
#include "MQTTUtil.h"

bool sendMQTTConnectAckMessage(MQTTConnection *connection, bool sessionPresent,
                               uint8_t returnCode) {
    MQTTFixedHeader fixedHeader;
    MQTTConnectAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_CONNACK << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTConnectAckVariableHeader);
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    variableHeader.flags = 0;
    if (sessionPresent) {
        variableHeader.flags |= MQTT_CONNACK_SESSION_PRESENT_MASK;
    }
    variableHeader.returnCode = returnCode;

    if (!connection->write((uint8_t *)&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    return true;
}