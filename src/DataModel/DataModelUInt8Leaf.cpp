#include "DataModelUInt8Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

DataModelUInt8Leaf::DataModelUInt8Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelUInt8Leaf & DataModelUInt8Leaf::operator = (const uint8_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        *this << (uint32_t)value;
    }

    return *this;
}

DataModelUInt8Leaf DataModelUInt8Leaf::operator ++ (int) {
    value++;
    updated();
    *this << (uint32_t)value;
    return *this;
}

DataModelUInt8Leaf DataModelUInt8Leaf::operator -- (int) {
    value--;
    updated();
    *this << (uint32_t)value;
    return *this;
}

DataModelUInt8Leaf::operator uint8_t() const {
    return value;
}


void DataModelUInt8Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<3> valueStr;
        etl::to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
