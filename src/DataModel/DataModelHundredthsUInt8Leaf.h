#ifndef DATA_MODEL_HUNDREDTHS_UINT8_LEAF_H
#define DATA_MODEL_HUNDREDTHS_UINT8_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelHundredthsUInt8Leaf : public DataModelRetainedValueLeaf {
   private:
        uint8_t wholeNumber;
        uint8_t hundredths;

    public:
        DataModelHundredthsUInt8Leaf(const char *name, DataModelElement *parent);
        void set(uint8_t wholeNumber, uint8_t hundredths);
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
