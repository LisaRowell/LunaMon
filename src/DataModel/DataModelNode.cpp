#include "DataModelNode.h"
#include "DataModelElement.h"

#include <Arduino.h>

DataModelNode::DataModelNode(const char *name, DataModelElement *parent,
                             DataModelElement *children[])
    : DataModelElement(name, parent), children(children) {
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
    bool atLeastOneMatch = false;
    for (childIndex = 0; children[childIndex] != NULL; childIndex++) {
        DataModelElement *child = children[childIndex];
        if (child->subscribeIfMatching(topicFilter, subscriber, cookie)) {
            atLeastOneMatch = true;
        }
    }

    return atLeastOneMatch;
}

void DataModelNode::unsubscribeIfMatching(const char *topicFilter,
                                          DataModelSubscriber &subscriber) {
    if (isMultiLevelWildcard(topicFilter)) {
        unsubscribeAll(subscriber);
        return;
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            // If we had non-leaf elements hold values, we'd do something interesting here, but
            // that's currently not a thing in the data model.
        } else {
            const char *newTopicFilter = topicFilter + offsetToNextLevel;
            unsubscribeChildrenIfMatching(newTopicFilter, subscriber);
        }
    }
}

void DataModelNode::unsubscribeChildrenIfMatching(const char *topicFilter,
                                                DataModelSubscriber &subscriber) {
    unsigned childIndex;
    for (childIndex = 0; children[childIndex] != NULL; childIndex++) {
        DataModelElement *child = children[childIndex];
        child->unsubscribeIfMatching(topicFilter, subscriber);
    }
}

void DataModelNode::unsubscribeAll(DataModelSubscriber &subscriber) {
    //If we allowed intermediate nodes to hold values, we would need to do an unsubscribe here.

    unsigned childIndex;
    for (childIndex = 0; children[childIndex] != NULL; childIndex++) {
        DataModelElement *child = children[childIndex];
        child->unsubscribeAll(subscriber);
    }
}
