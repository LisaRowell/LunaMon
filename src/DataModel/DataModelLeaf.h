#ifndef DATA_MODEL_LEAF_H
#define DATA_MODEL_LEAF_H

#include "DataModelElement.h"

class DataModelLeaf : public DataModelElement {
    protected:
        DataModelSubscriber *subscribers[maxDataModelSubscribers];
        uint32_t cookies[maxDataModelSubscribers];

        bool addSubscriber(DataModelSubscriber &subscriber, uint32_t cookie);
        virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(DataModelSubscriber &subscriber);
        void publish(const char *value);
        void publishToSubscriber(DataModelSubscriber &subscriber, const char *value,
                                 bool retainedValue);

    public:
        DataModelLeaf(const char *name, DataModelElement *parent);
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) override;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
        virtual void unsubscribeIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber) override;
        virtual void unsubscribeAll(DataModelSubscriber &subscriber) override;
};

#endif