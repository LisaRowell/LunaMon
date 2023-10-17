#include "DataModelBoolLeaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/to_string.h>

#include <stdint.h>

DataModelBoolLeaf::DataModelBoolLeaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelBoolLeaf & DataModelBoolLeaf::operator = (const bool value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        if (value) {
            *this << 1;
        } else {
            *this << 0;
        }
    }

    return *this;
}

DataModelBoolLeaf::operator bool() const {
    return value;
}


void DataModelBoolLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<2> valueStr;
        if (value) {
            valueStr = "1";
        } else {
            valueStr = "0";
        }
        publishToSubscriber(subscriber, valueStr, true);
    }
}
