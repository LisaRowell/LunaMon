#ifndef DATA_MODEL_NODE_H
#define DATA_MODEL_NODE_H

#include "DataModelElement.h"

class DataModelNode : public DataModelElement {
  private:
    // Pointer to a static, null terminated array of children.
    const DataModelElement *children;

  public:
    DataModelNode(const char *name, const DataModelElement *children);
};

#endif