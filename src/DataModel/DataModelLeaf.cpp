#include <Arduino.h>

#include "DataModelLeaf.h"
#include "DataModel.h"
#include "Util/Logger.h"

DataModelLeaf::DataModelLeaf(const char *name, DataModelElement *parent)
    : DataModelElement(name, parent) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        subscribers[subscriberPos] = NULL;
    }
}

bool DataModelLeaf::addSubscriber(DataModelSubscriber &subscriber, uint32_t cookie) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == NULL) {
            subscribers[subscriberPos] = &subscriber;
            cookies[subscriberPos] = cookie;
            return true;
        }
    }

    // This shouldn't happen if max sessions == max subscribers
    return false;
}

void DataModelLeaf::unsubscribe(DataModelSubscriber &subscriber) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == &subscriber) {
            subscribers[subscriberPos] = NULL;
            logger << logDebugDataModel << "Client '" << subscriber.name()
                   << "' unscribed from topic ending in '" << elementName() << "'" << eol;
            return;
        }
    }

    // MQTT protocol wide, tt's an acceptable occurance for a broker to receive an unsubscribe for
    // a topic that it doesn't have an active subcription for. We didn't find the subscriber on the
    // topic, but just calmly return.
}

bool DataModelLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    return addSubscriber(subscriber, cookie);
}

bool DataModelLeaf::subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                        uint32_t cookie) {
    if (isMultiLevelWildcard(topicFilter)) {
        return subscribe(subscriber, cookie);
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            return subscribe(subscriber, cookie);
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool DataModelLeaf::subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) {
    return subscribe(subscriber, cookie);
}

DataModelLeaf & DataModelLeaf::operator << (const char *value) {
    unsigned subscriberIndex;
    for (subscriberIndex = 0; subscriberIndex < maxDataModelSubscribers; subscriberIndex++) {
        DataModelSubscriber *subscriber = subscribers[subscriberIndex];
        if (subscriber != NULL) {
            // This could be made more efficent by building a topic name outside of this loop
            // instead of down in the publish routine...
            publishToSubscriber(*subscriber, value, false);
        }
    }

    return *this;
}

DataModelLeaf & DataModelLeaf::operator << (uint32_t value) {
    char valueStr[12];
    snprintf(valueStr, 12, "%lu", value);
    *this << valueStr;

    return *this;
}

void DataModelLeaf::publishToSubscriber(DataModelSubscriber &subscriber, const char *value,
                                        bool retainedValue) {
    char topic[maxTopicNameLength];
    buildTopicName(topic);
    subscriber.publish(topic, value, retainedValue);
}

void DataModelLeaf::unsubscribeIfMatching(const char *topicFilter,
                                          DataModelSubscriber &subscriber) {
    if (isMultiLevelWildcard(topicFilter)) {
        unsubscribe(subscriber);
        return;
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            unsubscribe(subscriber);
        }
    }
}

void DataModelLeaf::unsubscribeAll(DataModelSubscriber &subscriber) {
    unsigned subscriberIndex;
    for (subscriberIndex = 0; subscriberIndex < maxDataModelSubscribers; subscriberIndex++) {
        if (subscribers[subscriberIndex] == &subscriber) {
            // We don't cache the full name of a topic, and instead store it in bits in the tree,
            // so we don't log the full name. If we switch to storing the name, this debug could be
            // made to be more specific
            logger << logDebugDataModel << "Unsubscribing Client '" << subscriber.name()
                   << "' from Topic ending in '" << elementName() << "'" << eol;

            subscribers[subscriberIndex] = NULL;
        }
    }
}
