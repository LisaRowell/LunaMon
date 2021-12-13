#include <Arduino.h>

#include "MQTTUtil.h"

bool mqttWriteRemainingLength(MQTTConnection *connection, uint32_t remainingLength) {
    do {
        uint8_t encodedByte;
        encodedByte = remainingLength % 0x80;
        remainingLength = remainingLength / 0x80;
        if (remainingLength) {
            encodedByte |= 0x80;
        }
        if (!connection->write(&encodedByte, sizeof(uint8_t))) {
            return false;
        }
    } while (remainingLength);

    return true;
}

bool mqttWriteUInt16(MQTTConnection *connection, uint16_t value) {
    uint8_t valueBytes[2];
    valueBytes[0] = value >> 8;
    valueBytes[1] = value & 0xff;

    if (!connection->write(valueBytes, 2)) {
        return false;
    }

    return true;
}

bool mqttWriteMQTTString(MQTTConnection *connection, const char *string) {
    const uint16_t length = strlen(string);
    if (!mqttWriteUInt16(connection, length)) {
        return false;
    }

    return connection->write((uint8_t *)string, length);
}