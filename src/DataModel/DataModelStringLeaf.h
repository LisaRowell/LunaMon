#ifndef DATA_MODEL_STRING_LEAF_H
#define DATA_MODEL_STRING_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <etl/string.h>

#include <stddef.h>

using etl::istring;

class DataModelStringLeaf : public DataModelRetainedValueLeaf {
    private:
        istring &value;

    public:
        DataModelStringLeaf(const char *name, DataModelElement *parent, istring &buffer);
        DataModelStringLeaf & operator = (const istring &newString);
        DataModelStringLeaf & operator = (const char *newString);
        DataModelStringLeaf & operator = (const DataModelStringLeaf &otherLeaf);
        operator const char * () const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
        bool isEmptyStr() const;
        size_t maxLength() const;
};

#endif
