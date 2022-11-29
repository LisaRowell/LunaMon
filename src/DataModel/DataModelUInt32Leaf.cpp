#include "DataModelUInt32Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

DataModelUInt32Leaf::DataModelUInt32Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelUInt32Leaf & DataModelUInt32Leaf::operator = (const uint32_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        *this << value;
    }

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

DataModelUInt32Leaf::operator uint32_t() const {
    return value;
}


void DataModelUInt32Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<12> valueStr;
        etl::to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
