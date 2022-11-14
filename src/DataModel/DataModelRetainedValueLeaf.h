#ifndef DATA_MODEL_RETAINED_VALUE_LEAF_H
#define DATA_MODEL_RETAINED_VALUE_LEAF_H

#include "DataModelLeaf.h"

#include <stdint.h>

class DataModelRetainedValueLeaf : public DataModelLeaf {
    private:
        bool hasBeenSet;

    protected:
        DataModelRetainedValueLeaf(const char *name, DataModelElement *parent);
        virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie) override;
        void updated();
        bool hasValue() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) = 0;

    public:
        void removeValue();
};

#endif
