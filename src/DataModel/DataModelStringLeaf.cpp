#include "DataModelStringLeaf.h"

#include <stddef.h>

static char buffer[101];
DataModelStringLeaf::DataModelStringLeaf(const char *name, DataModelElement *parent,
                                         istring &buffer)
    : DataModelRetainedValueLeaf(name, parent), value(buffer) {
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const char *newString) {
    value = newString;

    updated();
    *this << newString;

    return *this;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const DataModelStringLeaf &otherLeaf) {
    this->value = otherLeaf.value;
    updated();
    *this << value.c_str();

    return *this;
}

DataModelStringLeaf::operator const char * () const {
    return value.c_str();
}


void DataModelStringLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        publishToSubscriber(subscriber, value.c_str(), true);
    }
}

bool DataModelStringLeaf::isEmptyStr() const {
    return hasValue() && value.empty();
}
