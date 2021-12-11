#include <Arduino.h>

#include "DataModelLeaf.h"

DataModelLeaf::DataModelLeaf(const char *name) : DataModelElement(name), hasBeenSet(false) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        subscribers[subscriberPos] = NULL;
    }
}

bool DataModelLeaf::hasValue() {
    return hasBeenSet;
}

void DataModelLeaf::updated() {
    hasBeenSet = true;

    // We'll do notifications here.
}

bool DataModelLeaf::subscribe(DataModelSubscriber *subscriber) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == NULL) {
            subscribers[subscriberPos] = subscriber;
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