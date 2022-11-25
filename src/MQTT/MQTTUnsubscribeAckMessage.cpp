#include "MQTTUnsubscribeAckMessage.h"
#include "MQTTConnection.h"
#include "MQTTUtil.h"

#include <stdint.h>

bool sendMQTTUnsubscribeAckMessage(MQTTConnection *connection, uint16_t packetId) {
    MQTTFixedHeader fixedHeader;
    MQTTUnsubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_UNSUBACK << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTUnsubscribeAckVariableHeader);
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (!connection->write((uint8_t *)&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    return true;
}
