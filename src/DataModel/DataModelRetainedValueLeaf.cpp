#include "DataModelRetainedValueLeaf.h"
#include "DataModelLeaf.h"

#include <stdint.h>

DataModelRetainedValueLeaf::DataModelRetainedValueLeaf(const char *name, DataModelElement *parent)
    : DataModelLeaf(name, parent), hasBeenSet(false) {
}

bool DataModelRetainedValueLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    if (!addSubscriber(subscriber, cookie)) {
        return false;
    }

    sendRetainedValue(subscriber);

    return true;
}

bool DataModelRetainedValueLeaf::hasValue() const {
    return hasBeenSet;
}

void DataModelRetainedValueLeaf::updated() {
    hasBeenSet = true;
}
