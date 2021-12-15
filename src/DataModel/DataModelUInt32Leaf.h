#ifndef DATA_MODEL_UINT32_LEAF_H
#define DATA_MODEL_UINT32_LEAF_H

#include "DataModelRetainedValueLeaf.h"

class DataModelUInt32Leaf : public DataModelRetainedValueLeaf {
   private:
        uint32_t value;

    public:
        DataModelUInt32Leaf(const char *name, DataModelElement *parent);
        void setValue(uint32_t value);
        void increment();
        void decrement();
        uint32_t currentValue();
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif