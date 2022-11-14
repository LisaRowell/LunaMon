#ifndef DATA_MODEL_HUNDREDTHS_UINT16_LEAF_H
#define DATA_MODEL_HUNDREDTHS_UINT16_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelHundredthsUInt16Leaf : public DataModelRetainedValueLeaf {
   private:
        uint16_t wholeNumber;
        uint8_t hundredths;

    public:
        DataModelHundredthsUInt16Leaf(const char *name, DataModelElement *parent);
        void set(uint16_t wholeNumber, uint8_t hundredths);
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
