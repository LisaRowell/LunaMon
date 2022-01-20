#include "MQTTUnsubscribeMessage.h"
#include "MQTTMessage.h"
#include "MQTTString.h"

#include "Util/Logger.h"

#include <Arduino.h>

MQTTUnsubscribeMessage::MQTTUnsubscribeMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

bool MQTTUnsubscribeMessage::parse() {
    if (fixedHeaderFlags() != 0x2) {
        logger << logWarning << "Received MQTT UNSUBSCRIBE message with invalid Fixed Header Flags"
               << eol;
        return false;
    }

    if (bytesAfterFixedHdr < sizeof(MQTTUnsubscribeVariableHeader)) {
        logger << logWarning
               << "Received MQTT UNSUBSCRIBE message with a size too small for the Variable Header."
               << eol;
        return false;
    }

    variableHeader = (MQTTUnsubscribeVariableHeader *)variableHeaderStart;
    const uint32_t bytesAfterVariableHdr = bytesAfterFixedHdr
                                            - sizeof(MQTTUnsubscribeVariableHeader);

    if (packetId() == 0) {
        logger << logWarning << "Received MQTT UNSUBSCRIBE message with zero Packet Indentifier."
               << eol;
        return false;
    }

    if (bytesAfterVariableHdr == 0) {
        logger << logWarning << "Received MQTT UNSUBSCRIBE message without any Topic Filters."
               << eol;
        return false;
    }

    payloadStart = variableHeaderStart + sizeof(MQTTUnsubscribeVariableHeader);
    uint8_t *payloadPos = payloadStart;
    uint32_t payloadBytesRemaining = bytesAfterVariableHdr;

    // Run through the topic filters making sure the message is properly formed.
    topicFilters = 0;
    do {
        MQTTString *topicFilterStr;
        if (!parseString(topicFilterStr, payloadPos, payloadBytesRemaining)) {
            logger << logWarning
                   << "MQTT UNSUBSCRIBE message with payload too small for its Topic Filter" << eol;
            return false;
        }

        if (topicFilterStr->length() == 0) {
            logger << logWarning << "MQTT UNSUBSCRIBE message with zero length Topic Filter" << eol;
            return false;
        }

        topicFilters++;
    } while (payloadBytesRemaining);

    topicFiltersPos = payloadStart;
    topicFiltersReturned = 0;

    return true;
}

bool MQTTUnsubscribeMessage::getTopicFilter(MQTTString * &topicFilter) {
    if (topicFiltersReturned == topicFilters) {
        return false;
    }

    grabString(topicFilter, topicFiltersPos);

    topicFiltersReturned++;

    return true;
}

uint16_t MQTTUnsubscribeMessage::packetId() const {
    return variableHeader->packetIdMSB * 256 + variableHeader->packetIdLSB;
}

unsigned MQTTUnsubscribeMessage::numTopicFilters() const {
    return topicFilters;
}
