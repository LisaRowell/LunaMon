#include "DataModelInt8Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

constexpr size_t maxStringLength = 4;

DataModelInt8Leaf::DataModelInt8Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelInt8Leaf & DataModelInt8Leaf::operator = (const int8_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        etl::string<maxStringLength> valueStr;
        etl::to_string(value, valueStr);
        *this << valueStr;
    }

    return *this;
}

DataModelInt8Leaf DataModelInt8Leaf::operator ++ (int) {
    value++;
    updated();
    etl::string<maxStringLength> valueStr;
    etl::to_string(value, valueStr);
    *this << valueStr;
    return *this;
}

DataModelInt8Leaf DataModelInt8Leaf::operator -- (int) {
    value--;
    updated();
    etl::string<maxStringLength> valueStr;
    etl::to_string(value, valueStr);
    *this << valueStr;
    return *this;
}

DataModelInt8Leaf::operator int8_t() const {
    return value;
}

void DataModelInt8Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        etl::to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
