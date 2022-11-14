#include "DataModelStringLeaf.h"

#include <stddef.h>

DataModelStringLeaf::DataModelStringLeaf(const char *name, DataModelElement *parent,
                                         istring &buffer)
    : DataModelRetainedValueLeaf(name, parent), value(buffer) {
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const istring &newString) {
    if (!hasValue() || value.compare(newString) != 0) {
        value = newString;
        updated();
        *this << value;
    }

    return *this;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const char *newString) {
    if (!hasValue() || value.compare(newString) != 0) {
        value = newString;
        updated();
        *this << value;
    }

    return *this;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const DataModelStringLeaf &otherLeaf) {
    if (!hasValue() || value.compare(otherLeaf.value) != 0) {
        this->value = otherLeaf.value;
        updated();
        *this << value;
    }

    return *this;
}

DataModelStringLeaf::operator const char * () const {
    return value.c_str();
}


void DataModelStringLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        publishToSubscriber(subscriber, value, true);
    }
}

bool DataModelStringLeaf::isEmptyStr() const {
    return hasValue() && value.empty();
}
