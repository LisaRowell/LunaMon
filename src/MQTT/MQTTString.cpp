#include "MQTTString.h"

#include <etl/string.h>

#include <stdint.h>

uint16_t MQTTString::length() const {
    return lengthMSB * 256 + lengthLSB;
}

uint32_t MQTTString::size() const {
    return length() + sizeof(MQTTString);
}

// Returns false if too long to copy. maxLength is max in source and does not include nil.
bool MQTTString::copyTo(char *cString, unsigned maxLength) const {
    const unsigned length = this->length();
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

// Returns false if too long to copy. maxLength is max in source and does not include nil.
bool MQTTString::copyTo(etl::istring &destString) const {
    const uint16_t length = this->length();
    if (length > destString.max_size()) {
        return false;
    }

    destString.assign(characterData, length);

    return true;
}
