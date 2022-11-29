#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

constexpr size_t maxStringLength = 7;

DataModelTenthsUInt16Leaf::DataModelTenthsUInt16Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

void DataModelTenthsUInt16Leaf::set(uint16_t wholeNumber, uint8_t tenths) {
    if (!hasValue() || this->wholeNumber != wholeNumber || this->tenths != tenths) {
        this->wholeNumber = wholeNumber;
        this->tenths = tenths;
        updated();
        etl::string<maxStringLength> valueStr;
        etl::string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        *this << valueStr;
    }
}

void DataModelTenthsUInt16Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        etl::string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        publishToSubscriber(subscriber, valueStr, true);
    }
}
