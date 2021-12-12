#include <Arduino.h>

#include "DataModelLeaf.h"

DataModelLeaf::DataModelLeaf(const char *name) : DataModelElement(name) {
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

void DataModelLeaf::unsubscribe(DataModelSubscriber *subscriber) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == subscriber) {
            subscribers[subscriberPos] = NULL;
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