#ifndef DATA_MODEL_ROOT_H
#define DATA_MODEL_ROOT_H

#include "DataModelNode.h"

class DataModelRoot : public DataModelNode {
    private:
        bool checkTopicFilterValidity(const char *topicFilter);

    public:
        DataModelRoot(DataModelElement **children);
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
};

#endif