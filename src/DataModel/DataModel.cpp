#include <Arduino.h>

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelRoot.h"
#include "DataModelUInt32Leaf.h"
#include "Config.h"
#include "Util/Logger.h"
#include "Util/Error.h"

DataModelUInt32Leaf mqttSessions("sessions", &mqttNode);
DataModelUInt32Leaf mqttConnections("connections", &mqttNode);

DataModelElement *mqttNodeChildren[] = { &mqttSessions, &mqttConnections, NULL};
DataModelNode mqttNode("MQTT", &controllerIDNode, mqttNodeChildren);

// Up time of this controller in seconds
DataModelLeaf controllerUpTime("upTime", &controllerIDNode);

DataModelElement *controllerIDNodeChildren[] = { &controllerUpTime, &mqttNode, NULL };
DataModelNode controllerIDNode(controllerID, &controllersNode, controllerIDNodeChildren);

DataModelElement *controllersNodeChildren[] = { &controllerIDNode, NULL };
DataModelNode controllersNode("controllers", &electronicsNode, controllersNodeChildren);

DataModelElement *electronicsNodeChildren[] = { &controllersNode, NULL };
DataModelNode electronicsNode("electronics", &dataModelRoot, electronicsNodeChildren);

DataModelElement *topNodeChildren[] = { &electronicsNode, NULL };
DataModelRoot dataModelRoot(topNodeChildren);

DataModel::DataModel() : root(dataModelRoot) {
}

bool DataModel::subscribe(const char *topicFilter, DataModelSubscriber &subscriber,
                          uint32_t cookie) {
    return root.subscribe(topicFilter, subscriber, cookie);
}

void DataModel::unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber) {
    root.unsubscribe(topicFilter, subscriber);
}

void DataModel::unsubscribeAll(DataModelSubscriber &subscriber) {
    root.unsubscribeAll(subscriber);
}
