#include <Arduino.h>

#include "DataModelUInt32Leaf.h"

#include "DataModelLeaf.h"

DataModelUInt32Leaf::DataModelUInt32Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelUInt32Leaf & DataModelUInt32Leaf::operator = (const uint32_t value) {
    this->value = value;
    updated();
    *this << value;
    return *this;
}

DataModelUInt32Leaf DataModelUInt32Leaf::operator ++ (int) {
    value++;
    updated();
    *this << value;
    return *this;
}

DataModelUInt32Leaf DataModelUInt32Leaf::operator -- (int) {
    value--;
    updated();
    *this << value;
    return *this;
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