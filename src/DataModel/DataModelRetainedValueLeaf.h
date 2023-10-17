#ifndef DATA_MODEL_RETAINED_VALUE_LEAF_H
#define DATA_MODEL_RETAINED_VALUE_LEAF_H

#include "DataModelLeaf.h"

#include <stdint.h>

class DataModelRetainedValueLeaf : public DataModelLeaf {
    private:
        bool hasBeenSet;
        static uint16_t retainedValues;

    protected:
        DataModelRetainedValueLeaf(const char *name, DataModelElement *parent);
        virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie) override;
        void updated();
        bool hasValue() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) = 0;

    public:
        void removeValue();
        static uint16_t retainedValueCount();
};

#endif
