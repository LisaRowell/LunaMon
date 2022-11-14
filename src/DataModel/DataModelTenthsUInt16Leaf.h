#ifndef DATA_MODEL_TENTHS_UINT16_LEAF_H
#define DATA_MODEL_TENTHS_UINT16_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelTenthsUInt16Leaf : public DataModelRetainedValueLeaf {
   private:
        uint16_t wholeNumber;
        uint8_t tenths;

    public:
        DataModelTenthsUInt16Leaf(const char *name, DataModelElement *parent);
        void set(uint16_t wholeNumber, uint8_t tenths);
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
