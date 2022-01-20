#include "DataModelRoot.h"
#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"

#include "Util/Logger.h"

#include <Arduino.h>

DataModelRoot::DataModelRoot(DataModelElement **children) : DataModelNode(NULL, NULL, children) {
}

bool DataModelRoot::subscribe(const char *topicFilter, DataModelSubscriber &subscriber,
                              uint32_t cookie) {
    if (!checkTopicFilterValidity(topicFilter )) {
        logger << logWarning << "Illegal Topic Filter '" << topicFilter << "'" << eol;
        return false;
    }

    if (isMultiLevelWildcard(topicFilter)) {
        return subscribeAll(subscriber, cookie);
    }

    return subscribeChildrenIfMatching(topicFilter, subscriber, cookie);
}

void DataModelRoot::unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber) {
    if (!checkTopicFilterValidity(topicFilter )) {
        logger << logWarning << "Illegal Topic Filter '" << topicFilter
               << "' in unsubscribe from Client '" << subscriber.name() << eol;
        return;
    }

    if (isMultiLevelWildcard(topicFilter)) {
        unsubscribeAll(subscriber);
    } else {
        unsubscribeChildrenIfMatching(topicFilter, subscriber);
    }
}

bool DataModelRoot::checkTopicFilterValidity(const char *topicFilter) {
    if (topicFilter[0] == 0) {
        return false;
    }

    unsigned pos;
    char previousChar;
    char currentChar;
    char nextChar;
    for (pos = 0, previousChar = 0, currentChar = topicFilter[0], nextChar = topicFilter[1];
         topicFilter[pos];
         pos++, previousChar = currentChar, currentChar = nextChar,
         nextChar = topicFilter[pos + 1]) {
        switch (currentChar) {
            case dataModelMultiLevelWildcard:
                return (previousChar == 0 || previousChar == dataModelLevelSeparator) &&
                       (nextChar == 0);

            case dataModelSingleLevelWildcard:
                if (previousChar != 0 && previousChar != dataModelLevelSeparator) {
                    return false;
                }
                if (nextChar != 0 && nextChar != dataModelLevelSeparator) {
                    return false;
                }
                break;

            case dataModelLevelSeparator:
                if (previousChar == dataModelLevelSeparator) {
                    return false;
                }
                break;

            default:
                break;
        }
    }

    return true;
}
