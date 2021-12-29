#include <Arduino.h>

#include "MQTTString.h"

uint16_t MQTTString::length() const {
    return lengthMSB * 256 + lengthLSB;
}

uint32_t MQTTString::size() const {
    return length() + sizeof(MQTTString);
}

// Returns false if too long to copy. maxLength is max in source and does not include nil.
bool MQTTString::copyTo(char *cString, unsigned maxLength) const {
    const unsigned length = MQTTString::length();
    if (length > maxLength) {
        return false;
    }

    unsigned pos;
    for (pos = 0; pos < length; pos++) {
        cString[pos] = characterData[pos];
    }
    cString[pos] = 0;

    return true;
}
