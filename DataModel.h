#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelClient.h"

extern DataModelUInt32Leaf controllerUpTime;
extern const DataModelNode controllerIDNode;

extern const DataModelNode controllersNode;

extern const DataModelNode electronicsNode;

extern const DataModelNode vesselNode;
extern const DataModelNode dataModelTopNode;

const unsigned maxDataModelClients = 2;

class DataModel {
  public:
    DataModel();
};

#endif