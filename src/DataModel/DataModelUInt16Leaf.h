#ifndef DATA_MODEL_UINT16_LEAF_H
#define DATA_MODEL_UINT16_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelUInt16Leaf : public DataModelRetainedValueLeaf {
   private:
        uint8_t value;

    public:
        DataModelUInt16Leaf(const char *name, DataModelElement *parent);
        DataModelUInt16Leaf & operator = (const uint16_t value);
        DataModelUInt16Leaf operator ++ (int);
        DataModelUInt16Leaf operator -- (int);
        operator uint16_t() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
