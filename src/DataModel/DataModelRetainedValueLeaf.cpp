#include "DataModelRetainedValueLeaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>

#include <stdint.h>

uint16_t DataModelRetainedValueLeaf::retainedValues = 0;

uint16_t DataModelRetainedValueLeaf::retainedValueCount() {
    return retainedValues;
}

DataModelRetainedValueLeaf::DataModelRetainedValueLeaf(const char *name, DataModelElement *parent)
    : DataModelLeaf(name, parent), hasBeenSet(false) {
}

bool DataModelRetainedValueLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    if (!DataModelLeaf::subscribe(subscriber, cookie)) {
        return false;
    }

    sendRetainedValue(subscriber);

    return true;
}

void DataModelRetainedValueLeaf::updated() {
    if (!hasBeenSet) {
        retainedValues++;
        hasBeenSet = true;
    }
}

void DataModelRetainedValueLeaf::removeValue() {
    if (hasBeenSet) {
        etl::string<1> emptyStr;

        *this << emptyStr;
        hasBeenSet = false;
    }
}

bool DataModelRetainedValueLeaf::hasValue() const {
    return hasBeenSet;
}
