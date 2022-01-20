#ifndef MQTT_SUBSCRIBE_MESSAGE_H
#define MQTT_SUBSCRIBE_MESSAGE_H

#include "MQTTMessage.h"
#include "MQTTString.h"

#include <Arduino.h>

struct MQTTSubscribeVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

class MQTTSubscribeMessage : MQTTMessage {
    private:
        MQTTSubscribeVariableHeader *variableHeader;
        uint8_t *payloadStart;
        unsigned topicFilters;
        unsigned topicFiltersReturned;
        uint8_t *topicFiltersPos;

    public:
        MQTTSubscribeMessage(MQTTMessage const &message);
        bool parse();
        bool getTopicFilter(MQTTString * &topicFilter, uint8_t &maxQoS);
        uint16_t packetId() const;
        unsigned numTopicFilters() const;
};

#endif
