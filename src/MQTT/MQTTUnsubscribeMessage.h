#ifndef MQTT_UNSUBSCRIBE_MESSAGE_H
#define MQTT_UNSUBSCRIBE_MESSAGE_H

#include "MQTTMessage.h"
#include "MQTTString.h"

#include <Arduino.h>

struct MQTTUnsubscribeVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

class MQTTUnsubscribeMessage : MQTTMessage {
    private:
        MQTTUnsubscribeVariableHeader *variableHeader;
        uint8_t *payloadStart;
        unsigned topicFilters;
        unsigned topicFiltersReturned;
        uint8_t *topicFiltersPos;

    public:
        MQTTUnsubscribeMessage(MQTTMessage const &message);
        bool parse();
        bool getTopicFilter(MQTTString * &topicFilter);
        uint16_t packetId() const;
        unsigned numTopicFilters() const;
};

#endif
