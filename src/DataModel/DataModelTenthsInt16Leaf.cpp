#include "DataModelTenthsInt16Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>
#include <stddef.h>

constexpr size_t maxStringLength = 8;

DataModelTenthsInt16Leaf::DataModelTenthsInt16Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

void DataModelTenthsInt16Leaf::set(int16_t wholeNumber, uint8_t tenths) {
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

void DataModelTenthsInt16Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        etl::string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        publishToSubscriber(subscriber, valueStr, true);
    }
}
