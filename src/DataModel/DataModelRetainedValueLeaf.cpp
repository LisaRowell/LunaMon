#include <Arduino.h>

#include "DataModelRetainedValueLeaf.h"
#include "DataModelLeaf.h"

DataModelRetainedValueLeaf::DataModelRetainedValueLeaf(const char *name)
    : DataModelLeaf(name), hasBeenSet(false) {
}

bool DataModelRetainedValueLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    if (!addSubscriber(subscriber, cookie)) {
        return false;
    }


    return true;
}

bool DataModelRetainedValueLeaf::hasValue() {
    return hasBeenSet;
}

bool DataModelRetainedValueLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    // TODO: Implement
    return true;
}

void DataModelRetainedValueLeaf::updated() {
    hasBeenSet = true;

    // We'll do notifications here.
}
