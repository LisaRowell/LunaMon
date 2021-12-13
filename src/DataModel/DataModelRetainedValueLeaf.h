#ifndef DATA_MODEL_RETAINED_VALUE_LEAF_H
#define DATA_MODEL_RETAINED_VALUE_LEAF_H

#include "DataModelLeaf.h"

class DataModelRetainedValueLeaf : public DataModelLeaf {
   private:
    bool hasBeenSet;

   protected:
    DataModelRetainedValueLeaf(const char *name, DataModelElement *parent);
    virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie) override;
    void updated();
    bool hasValue();
    virtual void sendRetainedValue(DataModelSubscriber &subscriber) = 0;
};

#endif