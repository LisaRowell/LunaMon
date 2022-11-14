#ifndef DATA_MODEL_UINT8_LEAF_H
#define DATA_MODEL_UINT8_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelUInt8Leaf : public DataModelRetainedValueLeaf {
   private:
        uint8_t value;

    public:
        DataModelUInt8Leaf(const char *name, DataModelElement *parent);
        DataModelUInt8Leaf & operator = (const uint8_t value);
        DataModelUInt8Leaf operator ++ (int);
        DataModelUInt8Leaf operator -- (int);
        operator uint8_t() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
