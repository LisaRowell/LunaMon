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
