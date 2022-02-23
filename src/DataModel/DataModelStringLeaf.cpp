#include "DataModelStringLeaf.h"

#include <stddef.h>

DataModelStringLeaf::DataModelStringLeaf(const char *name, DataModelElement *parent, size_t length)
    : DataModelRetainedValueLeaf(name, parent), maxLength(length) {
    // Note about the following new: There's often cautions in the Arduino community about the use
    // of new due to the risk of fragmentation in a small dynamic memory environment. The following
    // new is a concern in that regard since theses structures get allocated at start time and are
    // never freed.
    string = new char[length];
    string[0] = 0;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const char *newString) {
    unsigned strPos;
    for (strPos = 0; strPos < maxLength - 1 && newString[strPos] != 0; strPos++) {
        string[strPos]  = newString[strPos];
    }
    this->string[strPos] = 0;

    updated();
    *this << newString;

    return *this;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const DataModelStringLeaf &otherLeaf) {
    unsigned strPos;
    for (strPos = 0; strPos < maxLength - 1 && otherLeaf.string[strPos] != 0; strPos++) {
        string[strPos]  = otherLeaf.string[strPos];
    }
    this->string[strPos] = 0;

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
