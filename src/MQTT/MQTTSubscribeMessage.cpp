#include "MQTTSubscribeMessage.h"
#include "MQTTMessage.h"
#include "MQTTString.h"

#include "Util/Logger.h"

#include <Arduino.h>

MQTTSubscribeMessage::MQTTSubscribeMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

bool MQTTSubscribeMessage::parse() {
    if (fixedHeaderFlags() != 0x2) {
        logger << logWarning << "Received MQTT SUBSCRIBE message with invalid Fixed Header Flags"
               << eol;
        return false;
    }

    if (bytesAfterFixedHdr < sizeof(MQTTSubscribeVariableHeader)) {
        logger << logWarning
               << "Received MQTT SUBSCRIBE message with a size too small for the Variable Header."
               << eol;
        return false;
    }

    variableHeader = (MQTTSubscribeVariableHeader *)variableHeaderStart;
    const uint32_t bytesAfterVariableHdr = bytesAfterFixedHdr - sizeof(MQTTSubscribeVariableHeader);

    if (packetId() == 0) {
        logger << logWarning << "Received MQTT SUBSCRIBE message with zero Packet Indentifier."
               << eol;
        return false;
    }

    if (bytesAfterVariableHdr == 0) {
        logger << logWarning << "Received MQTT SUBSCRIBE message without any Topic Filters." << eol;
        return false;
    }

    payloadStart = variableHeaderStart + sizeof(MQTTSubscribeVariableHeader);
    uint8_t *payloadPos = payloadStart;
    uint32_t payloadBytesRemaining = bytesAfterVariableHdr;

    // Run through the topic filters making sure the message is properly formed.
    topicFilters = 0;
    do {
        MQTTString *topicFilterStr;
        if (!parseString(topicFilterStr, payloadPos, payloadBytesRemaining)) {
            logger << logWarning
                   << "MQTT SUBSCRIBE message with payload too small for its Topic Filter" << eol;
            return false;
        }

        if (topicFilterStr->length() == 0) {
            logger << logWarning << "MQTT SUBSCRIBE message with zero length Topic Filter" << eol;
            return false;
        }

        if (payloadBytesRemaining < 1) {
            logger << logWarning << "MQTT SUBSCRIBE message with a Topic Filter missing its max QoS"
                   << eol;
            return false;
        }
        uint8_t maxQoS = *payloadPos++;
        payloadBytesRemaining--;

        if (maxQoS > 2) {
            logger << logWarning << "MQTT SUBSCRIBE message with illegal max QoS (" << Hex << maxQoS
                   << ")" << eol;
            return false;
        }

        topicFilters++;
    } while (payloadBytesRemaining);

    topicFiltersPos = payloadStart;
    topicFiltersReturned = 0;

    return true;
}

bool MQTTSubscribeMessage::getTopicFilter(MQTTString * &topicFilter, uint8_t &maxQoS) {
    if (topicFiltersReturned == topicFilters) {
        return false;
    }

    grabString(topicFilter, topicFiltersPos);
    maxQoS = *topicFiltersPos++;

    topicFiltersReturned++;

    return true;
}

uint16_t MQTTSubscribeMessage::packetId() const {
    return variableHeader->packetIdMSB * 256 + variableHeader->packetIdLSB;
}

unsigned MQTTSubscribeMessage::numTopicFilters() const {
    return topicFilters;
}
