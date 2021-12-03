#include <Arduino.h>

#include "MQTTString.h"

uint16_t MQTTString::length() const {
  return lengthMSB * 256 + lengthLSB;
}

uint32_t MQTTString::size() const {
  return length() + sizeof(MQTTString);
}

// This is on the shitty side, but it's only for debug messages
// Delete later when log is implemented.
void MQTTString::print() const {
  uint16_t pos;
  const uint16_t length = MQTTString::length();
  for (pos = 0; pos < length; pos++) {
    Serial.print(characterData[pos]);
  }
}
