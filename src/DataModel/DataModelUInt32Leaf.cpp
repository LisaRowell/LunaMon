#include <Arduino.h>

#include "DataModelUInt32Leaf.h"

#include "DataModelLeaf.h"

DataModelUInt32Leaf::DataModelUInt32Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

void DataModelUInt32Leaf::setValue(uint32_t value) {
    DataModelUInt32Leaf::value = value;
    updated();

    char valueStr[12];
    snprintf(valueStr, 12, "%lu", value);
    publish(valueStr);
}

uint32_t DataModelUInt32Leaf::currentValue() {
    return value;
}

void DataModelUInt32Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
       char valueStr[12];
        snprintf(valueStr, 12, "%lu", value);
        publishToSubscriber(subscriber, valueStr, true);
    }
}