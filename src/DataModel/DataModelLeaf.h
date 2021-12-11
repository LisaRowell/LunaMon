#ifndef DATA_MODEL_LEAF_H
#define DATA_MODEL_LEAF_H

#include "DataModelElement.h"

class DataModelLeaf : public DataModelElement {
    protected:
        bool hasBeenSet;
        DataModelSubscriber *subscribers[maxDataModelSubscribers];

        void updated();

    public:
        DataModelLeaf(const char *name);
        bool hasValue();
        bool subscribe(DataModelSubscriber *subscriber);
        void unsubscribe(DataModelSubscriber *subscriber);
};

#endif