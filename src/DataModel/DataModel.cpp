#include <Arduino.h>

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelUInt32Leaf.h"
#include "Config.h"
#include "Util/Error.h"

// Up time of this controller in seconds
DataModelUInt32Leaf controllerUpTime("upTime");

const DataModelElement controllerIDNodeChildren[] = { controllerUpTime, NULL };
const DataModelNode controllerIDNode(controllerID, controllerIDNodeChildren);

const DataModelElement controllersNodeChildren[] = { controllerIDNode, NULL };
const DataModelNode controllersNode("controllers", controllersNodeChildren);

const DataModelElement electronicsNodeChildren[] = { controllersNode, NULL };
const DataModelNode electronicsNode("electronics", electronicsNodeChildren);

const DataModelElement vesselNodeChildren[] = { electronicsNode, NULL };
const DataModelNode vesselNode(mmsi, vesselNodeChildren);

const DataModelElement topNodeChildren[] = { vesselNode, NULL };
const DataModelNode dataModelTopNode("vessels", topNodeChildren);

DataModel::DataModel() {
}