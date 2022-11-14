#ifndef DATA_MODEL_INT8_LEAF_H
#define DATA_MODEL_INT8_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelInt8Leaf : public DataModelRetainedValueLeaf {
   private:
        int8_t value;

    public:
        DataModelInt8Leaf(const char *name, DataModelElement *parent);
        DataModelInt8Leaf & operator = (const int8_t value);
        DataModelInt8Leaf operator ++ (int);
        DataModelInt8Leaf operator -- (int);
        operator int8_t() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
