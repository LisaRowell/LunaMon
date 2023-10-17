#ifndef DATA_MODEL_BOOL_LEAF_H
#define DATA_MODEL_BOOL_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelBoolLeaf : public DataModelRetainedValueLeaf {
   private:
        bool value;

    public:
        DataModelBoolLeaf(const char *name, DataModelElement *parent);
        DataModelBoolLeaf & operator = (const bool value);
        operator bool() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif // DATA_MODEL_BOOL_LEAF_H
