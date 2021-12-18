#include <Arduino.h>

#include "DataModelStringLeaf.h"

DataModelStringLeaf::DataModelStringLeaf(const char *name, DataModelElement *parent, size_t length)
    : DataModelRetainedValueLeaf(name, parent), maxLength(length) {
    string = new char[length];
    string[0] = 0;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const char *string) {
    strncpy(this->string, string, maxLength);
    updated();
    *this << string;

    return *this;
}

DataModelStringLeaf::operator const char * () const {
    return string;
}


void DataModelStringLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        publishToSubscriber(subscriber, string, true);
    }
}

bool DataModelStringLeaf::isEmptyStr() const {
    return hasValue() && string[0] == 0;
}
