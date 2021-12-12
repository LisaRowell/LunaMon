#include <Arduino.h>

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelRoot.h"
#include "DataModelUInt32Leaf.h"
#include "Config.h"
#include "Util/Logger.h"
#include "Util/Error.h"

// Up time of this controller in seconds
DataModelUInt32Leaf controllerUpTime("upTime");

DataModelElement *controllerIDNodeChildren[] = { &controllerUpTime, NULL };
DataModelNode controllerIDNode(controllerID, controllerIDNodeChildren);

DataModelElement *controllersNodeChildren[] = { &controllerIDNode, NULL };
DataModelNode controllersNode("controllers", controllersNodeChildren);

DataModelElement *electronicsNodeChildren[] = { &controllersNode, NULL };
DataModelNode electronicsNode("electronics", electronicsNodeChildren);

DataModelElement *topNodeChildren[] = { &electronicsNode, NULL };
DataModelRoot dataModelRoot(topNodeChildren);

DataModel::DataModel() : root(dataModelRoot) {
}

bool DataModel::subscribe(const char *topicFilter, DataModelSubscriber &subscriber,
                          uint32_t cookie) {
    return root.subscribe(topicFilter, subscriber, cookie);
}
