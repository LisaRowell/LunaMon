#ifndef MQTT_STRING_H
#define MQTT_STRING_H

class MQTTString {
  private:
    uint8_t lengthMSB;
    uint8_t lengthLSB;
    char characterData[];

  public:
    uint16_t length() const;
    uint32_t size() const;
    void print() const;
};

#endif