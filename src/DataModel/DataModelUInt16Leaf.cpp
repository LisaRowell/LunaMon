#include "DataModelUInt16Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

using etl::string;
using etl::to_string;

DataModelUInt16Leaf::DataModelUInt16Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelUInt16Leaf & DataModelUInt16Leaf::operator = (const uint16_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        *this << (uint32_t)value;
    }

    return *this;
}

DataModelUInt16Leaf DataModelUInt16Leaf::operator ++ (int) {
    value++;
    updated();
    *this << (uint32_t)value;
    return *this;
}

DataModelUInt16Leaf DataModelUInt16Leaf::operator -- (int) {
    value--;
    updated();
    *this << (uint32_t)value;
    return *this;
}

DataModelUInt16Leaf::operator uint16_t() const {
    return value;
}


void DataModelUInt16Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        string<3> valueStr;
        to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
