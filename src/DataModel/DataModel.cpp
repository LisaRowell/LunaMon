#include <Arduino.h>

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelRoot.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelStringLeaf.h"
#include "Config.h"

#include "Util/Logger.h"
#include "Util/Error.h"

DataModelStringLeaf mqttSession1("1", &mqttSessionsNode, maxSessionDescriptionLength);
DataModelStringLeaf mqttSession2("2", &mqttSessionsNode, maxSessionDescriptionLength);
DataModelStringLeaf mqttSession3("3", &mqttSessionsNode, maxSessionDescriptionLength);
DataModelStringLeaf mqttSession4("4", &mqttSessionsNode, maxSessionDescriptionLength);
DataModelStringLeaf mqttSession5("5", &mqttSessionsNode, maxSessionDescriptionLength);

DataModelStringLeaf *mqttSessionDebugs[] = {
    &mqttSession1,
    &mqttSession2,
    &mqttSession3,
    &mqttSession4,
    &mqttSession5
};

DataModelElement *mqttSessionsNodeChildren[] = {
    &mqttSession1,
    &mqttSession2,
    &mqttSession3,
    &mqttSession4,
    &mqttSession5,
    NULL
};
DataModelNode mqttSessionsNode("sessions", &mqttNode, mqttSessionsNodeChildren);

DataModelStringLeaf mqttConnection1("1", &mqttConnectionsNode, maxConnectionDescriptionLength);
DataModelStringLeaf mqttConnection2("2", &mqttConnectionsNode, maxConnectionDescriptionLength);
DataModelStringLeaf mqttConnection3("3", &mqttConnectionsNode, maxConnectionDescriptionLength);
DataModelStringLeaf mqttConnection4("4", &mqttConnectionsNode, maxConnectionDescriptionLength);
DataModelStringLeaf mqttConnection5("5", &mqttConnectionsNode, maxConnectionDescriptionLength);

DataModelStringLeaf *mqttConnectionDebugs[] = {
    &mqttConnection1,
    &mqttConnection2,
    &mqttConnection3,
    &mqttConnection4,
    &mqttConnection5
};

DataModelElement *mqttConnectionsNodeChildren[] = {
    &mqttConnection1,
    &mqttConnection2,
    &mqttConnection3,
    &mqttConnection4,
    &mqttConnection5,
    NULL
};
DataModelNode mqttConnectionsNode("connections", &mqttNode, mqttConnectionsNodeChildren);

DataModelUInt32Leaf mqttSessionCount("sessionCount", &mqttNode);
DataModelUInt32Leaf mqttConnectionCount("connectionCount", &mqttNode);

DataModelElement *mqttNodeChildren[] = {
    &mqttSessionCount,
    &mqttConnectionCount,
    &mqttConnectionsNode,
    &mqttSessionsNode,
    NULL
};
DataModelNode mqttNode("MQTT", &controllerIDNode, mqttNodeChildren);

// Up time of this controller in seconds
DataModelUInt32Leaf controllerUpTime("upTime", &controllerIDNode);

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
