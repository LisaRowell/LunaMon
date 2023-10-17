#ifndef DATA_MODEL_ROOT_H
#define DATA_MODEL_ROOT_H

#include "DataModelNode.h"

#include <stdint.h>

class DataModelRoot : public DataModelNode {
    private:
        bool checkTopicFilterValidity(const char *topicFilter);
        bool subscribeChildrenIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie);

    public:
        DataModelRoot(DataModelElement **children);
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber);
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
};

#endif
