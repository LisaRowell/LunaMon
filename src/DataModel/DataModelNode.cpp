#include <Arduino.h>

#include "DataModelNode.h"
#include "DataModelElement.h"

DataModelNode::DataModelNode(const char *name, DataModelElement *children[])
    : DataModelElement(name), children(children) {
}

bool DataModelNode::subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) {
    unsigned childIndex;
    for (childIndex = 0; children[childIndex] != NULL; childIndex++) {
        DataModelElement *child = *children + childIndex;
        if (!child->subscribeAll(subscriber, cookie)) {
            return false;
        }
    }

    return true;
}

bool DataModelNode::subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                        uint32_t cookie) {
    if (isMultiLevelWildcard(topicFilter)) {
        return subscribeAll(subscriber, cookie);
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            // If we had non-leaf elements hold values, we'd do something interesting here, but
            // that's currently not a thing in the data model.
            return false;
        } else {
            const char *newTopicFilter = topicFilter + offsetToNextLevel;
            return subscribeChildrenIfMatching(newTopicFilter, subscriber, cookie);
        }
    } else {
        return false;
    }
}

bool DataModelNode::subscribeChildrenIfMatching(const char *topicFilter,
                                                DataModelSubscriber &subscriber,
                                                uint32_t cookie) {
    unsigned childIndex;
    for (childIndex = 0; children[childIndex] != NULL; childIndex++) {
        DataModelElement *child = *children + childIndex;
        if (!child->subscribeIfMatching(topicFilter, subscriber, cookie)) {
            return false;
        }
    }

    return true;
}