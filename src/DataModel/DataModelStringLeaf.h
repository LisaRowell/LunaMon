#ifndef DATA_MODEL_STRING_LEAF_H
#define DATA_MODEL_STRING_LEAF_H

#include <Arduino.h>

#include "DataModelRetainedValueLeaf.h"

class DataModelStringLeaf : public DataModelRetainedValueLeaf {
   private:
        char *string;
        size_t maxLength;

    public:
        DataModelStringLeaf(const char *name, DataModelElement *parent, size_t length);
        DataModelStringLeaf & operator = (const char *newString);
        DataModelStringLeaf & operator = (const DataModelStringLeaf &otherLeaf);
        operator const char * () const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
        bool isEmptyStr() const;
};

#endif
