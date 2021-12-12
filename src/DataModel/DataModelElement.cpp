#include <Arduino.h>

#include "DataModelElement.h"
#include "DataModel.h"
#include "Util/Logger.h"

DataModelElement::DataModelElement(const char *name) : name(name) {
}

bool DataModelElement::isMultiLevelWildcard(const char *topicFilter) {
    return (topicFilter[0] == dataModelMultiLevelWildcard) && (topicFilter[1] == 0);
}

bool DataModelElement::topicFilterMatch(const char *topicFilter, unsigned &offsetToNextLevel,
                                        bool &lastLevel) {
    if (topicFilter[0] == dataModelSingleLevelWildcard) {
        switch (topicFilter[1]) {
            case 0:
                offsetToNextLevel = 0;
                lastLevel = true;
                return true;

            case dataModelLevelSeparator:
                offsetToNextLevel = 2;
                lastLevel = topicFilter[offsetToNextLevel] == 0;
                return true;

            default:
                logger << logError
                       << "Data Model TopicFilter was not properly checked and went through invalid"
                       << eol;
                return false;
        }
    } else {
        unsigned pos;
        for (pos = 0;
             (topicFilter[pos] != dataModelLevelSeparator) && (topicFilter[pos] != 0);
             pos++) {
            if (name[pos] != topicFilter[pos]) {
                return false;
            }
        }

        offsetToNextLevel = pos + 1;
        lastLevel = topicFilter[pos] == 0;

        return true;
    }
}
