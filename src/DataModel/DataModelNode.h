#ifndef DATA_MODEL_NODE_H
#define DATA_MODEL_NODE_H

#include "DataModelElement.h"

class DataModelNode : public DataModelElement {
    private:
        // Pointer to a static, null terminated array of children.
        DataModelElement **children;

    protected:
        bool subscribeChildrenIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie);

    public:
        DataModelNode(const char *name, DataModelElement *parent, DataModelElement **children);
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) override;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
};

#endif