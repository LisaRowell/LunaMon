#include "DataModelTenthsInt16Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

using etl::string;
using etl::string_stream;
using etl::setfill;
using etl::setw;

constexpr size_t maxStringLength = 8;

DataModelTenthsInt16Leaf::DataModelTenthsInt16Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

void DataModelTenthsInt16Leaf::set(int16_t wholeNumber, uint8_t tenths) {
    if (!hasValue() || this->wholeNumber != wholeNumber || this->tenths != tenths) {
        this->wholeNumber = wholeNumber;
        this->tenths = tenths;
        updated();
        string<maxStringLength> valueStr;
        string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        *this << valueStr;
    }
}

void DataModelTenthsInt16Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        string<maxStringLength> valueStr;
        string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        publishToSubscriber(subscriber, valueStr, true);
    }
}
