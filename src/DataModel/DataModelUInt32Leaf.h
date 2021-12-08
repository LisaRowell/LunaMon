#ifndef DATA_MODEL_UINT32_LEAF_H
#define DATA_MODEL_UINT32_LEAF_H

#include "DataModelLeaf.h"

class DataModelUInt32Leaf : public DataModelLeaf {
  private:
    uint32_t value;

  public:
    DataModelUInt32Leaf(const char *name);
    void publish(uint32_t value);
    uint32_t currentValue();
};

#endif