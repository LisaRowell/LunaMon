#ifndef DATA_MODEL_NODE_H
#define DATA_MODEL_NODE_H

class DataModelSubscriber;

#include "DataModelElement.h"

#include <stdint.h>

class DataModelNode : public DataModelElement {
    private:
        // Pointer to a static, null terminated array of children.
        DataModelElement **children;

    protected:
        bool subscribeChildrenIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie);
        void unsubscribeChildrenIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber);

    public:
        DataModelNode(const char *name, DataModelElement *parent, DataModelElement **children);
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) override;
        virtual void unsubscribeIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber) override;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
        virtual void unsubscribeAll(DataModelSubscriber &subscriber) override;
};

#endif
