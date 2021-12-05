#include <Arduino.h>

#include "DataModelUInt32Leaf.h"
#include "DataModelLeaf.h"

DataModelUInt32Leaf::DataModelUInt32Leaf(const char *name) : DataModelLeaf(name) {
}

void DataModelUInt32Leaf::set(uint32_t value) {
  DataModelUInt32Leaf::value = value;
  updated();
}

uint32_t DataModelUInt32Leaf::currentValue() {
  return value;
}