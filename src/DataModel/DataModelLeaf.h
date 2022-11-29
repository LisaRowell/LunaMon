#ifndef DATA_MODEL_LEAF_H
#define DATA_MODEL_LEAF_H

class DataModelSubscriber;

#include "DataModelElement.h"

#include <etl/string.h>

#include <stdint.h>

class DataModelLeaf : public DataModelElement {
    private:
        DataModelSubscriber *subscribers[maxDataModelSubscribers];
        uint32_t cookies[maxDataModelSubscribers];
        bool addSubscriber(DataModelSubscriber &subscriber, uint32_t cookie);

    protected:
        bool isSubscribed(DataModelSubscriber &subscriber);
        bool updateSubscriber(DataModelSubscriber &subscriber, uint32_t cookie);
        virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(DataModelSubscriber &subscriber);
        void publishToSubscriber(DataModelSubscriber &subscriber, const etl::istring &value,
                                 bool retainedValue);

    public:
        DataModelLeaf(const char *name, DataModelElement *parent);
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) override;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
        virtual void unsubscribeIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber) override;
        virtual void unsubscribeAll(DataModelSubscriber &subscriber) override;
        DataModelLeaf & operator << (const etl::istring &value);
        DataModelLeaf & operator << (uint32_t value);
};

#endif
