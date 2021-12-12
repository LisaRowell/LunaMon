#ifndef MQTT_SUBSCRIBE_MESSAGE_H
#define MQTT_SUBSCRIBE_MESSAGE_H

#include "MQTTMessage.h"
#include "MQTTString.h"

struct MQTTSubscribeVariableHeader {
    uint8_t packetIDMSB;
    uint8_t packetIDLSB;
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
        uint16_t packetID() const;
        unsigned numTopicFilters() const;
};

#endif