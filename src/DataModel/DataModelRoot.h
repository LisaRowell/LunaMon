#ifndef DATA_MODEL_ROOT_H
#define DATA_MODEL_ROOT_H

#include "DataModelNode.h"

#include <Arduino.h>

class DataModelRoot : public DataModelNode {
    private:
        bool checkTopicFilterValidity(const char *topicFilter);

    public:
        DataModelRoot(DataModelElement **children);
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber);
};

#endif
