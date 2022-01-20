#include "MQTTPublishMessage.h"
#include "MQTTMessage.h"
#include "MQTTUtil.h"

#include "Util/Logger.h"

#include <Arduino.h>

bool sendMQTTPublishMessage(MQTTConnection *connection, const char *topic, const char *value,
                            bool dup, uint8_t qosLevel, bool retain,
                            uint16_t packetId) {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PUBLISH << MQTT_MSG_TYPE_SHIFT;
    if (dup) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_DUP_MASK;
    }
    fixedHeader.typeAndFlags |= qosLevel << MQTT_PUBLISH_FLAGS_QOS_SHIFT;
    if (retain) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_RETAIN_MASK;
    }
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }

    const size_t valueLength = strlen(value);
    uint32_t remainingLength;
    remainingLength = strlen(topic) + 2 + valueLength;
    if (qosLevel > 0) {
        remainingLength += 2;
    }
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    if (!mqttWriteMQTTString(connection, topic)) {
        return false;
    }

    if (qosLevel > 0) {
        if (!mqttWriteUInt16(connection, packetId)) {
            return false;
        }
    }

    return connection->write((uint8_t *)value, valueLength);
}
