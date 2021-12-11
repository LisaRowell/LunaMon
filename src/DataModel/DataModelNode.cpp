#include <Arduino.h>

#include "DataModelNode.h"
#include "DataModelElement.h"

DataModelNode::DataModelNode(const char *name, const DataModelElement *children)
    : DataModelElement(name), children(children) {
}
