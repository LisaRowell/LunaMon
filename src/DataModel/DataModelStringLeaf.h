#ifndef DATA_MODEL_STRING_LEAF_H
#define DATA_MODEL_STRING_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <etl/string.h>

#include <stddef.h>

class DataModelStringLeaf : public DataModelRetainedValueLeaf {
    private:
        etl::istring &value;

    public:
        DataModelStringLeaf(const char *name, DataModelElement *parent, etl::istring &buffer);
        DataModelStringLeaf & operator = (const etl::istring &newString);
        DataModelStringLeaf & operator = (const char *newString);
        DataModelStringLeaf & operator = (const DataModelStringLeaf &otherLeaf);
        operator const char * () const;
        int compare(const etl::istring &otherString) const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
        bool isEmptyStr() const;
        size_t maxLength() const;
};

#endif
