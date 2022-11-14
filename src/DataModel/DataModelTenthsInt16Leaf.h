#ifndef DATA_MODEL_TENTHS_INT16_LEAF_H
#define DATA_MODEL_TENTHS_INT16_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelTenthsInt16Leaf : public DataModelRetainedValueLeaf {
   private:
        int16_t wholeNumber;
        uint8_t tenths;

    public:
        DataModelTenthsInt16Leaf(const char *name, DataModelElement *parent);
        void set(int16_t wholeNumber, uint8_t tenths);
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
