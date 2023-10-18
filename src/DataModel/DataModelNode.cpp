/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataModelNode.h"
#include "DataModelElement.h"

#include "Util/Logger.h"

#include <stdint.h>

DataModelNode::DataModelNode(const char *name, DataModelElement *parent,
                             DataModelElement *children[])
    : DataModelElement(name, parent), children(children) {
}

bool DataModelNode::subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) {
    logger << logDebugDataModel << subscriber.name()
           << " subscribing to children of node ending in '" << elementName()
           << "' via subscription wildcard" << eol;

    unsigned childIndex;
    for (childIndex = 0; children[childIndex] != nullptr; childIndex++) {
        DataModelElement *child = children[childIndex];
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
    for (childIndex = 0; children[childIndex] != nullptr; childIndex++) {
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
    for (childIndex = 0; children[childIndex] != nullptr; childIndex++) {
        DataModelElement *child = children[childIndex];
        child->unsubscribeIfMatching(topicFilter, subscriber);
    }
}

void DataModelNode::unsubscribeAll(DataModelSubscriber &subscriber) {
    //If we allowed intermediate nodes to hold values, we would need to do an unsubscribe here.

    unsigned childIndex;
    for (childIndex = 0; children[childIndex] != nullptr; childIndex++) {
        DataModelElement *child = children[childIndex];
        child->unsubscribeAll(subscriber);
    }
}
