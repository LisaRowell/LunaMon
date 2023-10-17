#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelRoot.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"
#include "DataModelStringLeaf.h"
#include "Config.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsManager.h"

#include "Util/Logger.h"
#include "Util/Error.h"

#include <etl/string.h>

#include <stdint.h>

DataModelUInt32Leaf sysBrokerClientsConnected("connected", &sysBrokerClientsNode);
DataModelUInt32Leaf sysBrokerClientsDisconnected("disconnected", &sysBrokerClientsNode);
DataModelUInt32Leaf sysBrokerClientsMaximum("maximum", &sysBrokerClientsNode);
DataModelUInt32Leaf sysBrokerClientsTotal("total", &sysBrokerClientsNode);

DataModelElement *sysBrokerClientsChildren[] = {
    &sysBrokerClientsConnected,
    &sysBrokerClientsDisconnected,
    &sysBrokerClientsMaximum,
    &sysBrokerClientsTotal,
    NULL
};
DataModelNode sysBrokerClientsNode("clients", &sysBrokerNode, sysBrokerClientsChildren);

DataModelUInt32Leaf sysBrokerMessagesReceived("received", &sysBrokerMessagesNode);
DataModelUInt32Leaf sysBrokerMessagesSent("sent", &sysBrokerMessagesNode);

DataModelElement *sysBrokerMessagesChildren[] = {
    &sysBrokerMessagesReceived,
    &sysBrokerMessagesSent,
    NULL
};
DataModelNode sysBrokerMessagesNode("messages", &sysBrokerNode, sysBrokerMessagesChildren);

DataModelUInt32Leaf sysBrokerUptime("uptime", &sysBrokerNode);

DataModelElement *sysBrokerChildren[] = {
    &sysBrokerClientsNode,
    &sysBrokerMessagesNode,
    &sysBrokerUptime,
    NULL
};
DataModelNode sysBrokerNode("broker", &sysNode, sysBrokerChildren);

DataModelElement *sysNodeChildren[] = {
    &sysBrokerNode,
    NULL
};
DataModelNode sysNode("$SYS", &dataModelRoot, sysNodeChildren);

DataModelLeaf nmeaDataModelMessagesBridged("messages", &nmeaDataModelBridgeNode);
DataModelLeaf nmeaDataModelMessageBridgeRate("messageRate", &nmeaDataModelBridgeNode);

DataModelElement *nmeaDataModelBridgeNodeChildren[] = {
    &nmeaDataModelMessagesBridged,
    &nmeaDataModelMessageBridgeRate,
    NULL
};
DataModelNode nmeaDataModelBridgeNode("nmeaDataModelBridge", &controllerIDNode,
                                      nmeaDataModelBridgeNodeChildren);

DataModelLeaf dataModelLeafUpdates("updates", &dataModelNode);
DataModelLeaf dataModelLeafUpdateRate("updateRate", &dataModelNode);

DataModelElement *dataModelNodeChildren[] = {
    &dataModelLeafUpdates,
    &dataModelLeafUpdateRate,
    NULL
};
DataModelNode dataModelNode("dataModel", &controllerIDNode, dataModelNodeChildren);

DataModelLeaf wifiNMEAMessages("messages", &wifiNMEANode);
DataModelLeaf wifiNMEAMessageRate("messageRate", &wifiNMEANode);

DataModelElement *wifiNMEANodeChildren[] = {
    &wifiNMEAMessages,
    &wifiNMEAMessageRate,
    NULL
};
DataModelNode wifiNMEANode("wifi", &nmeaNode, wifiNMEANodeChildren);

DataModelLeaf usbNMEAMessages("messages", &usbNMEANode);
DataModelLeaf usbNMEAMessageRate("messageRate", &usbNMEANode);

DataModelElement *usbNMEANodeChildren[] = {
    &usbNMEAMessages,
    &usbNMEAMessageRate,
    NULL
};
DataModelNode usbNMEANode("usb", &nmeaNode, usbNMEANodeChildren);

DataModelElement *nmeaNodeChildren[] = {
    &usbNMEANode,
    &wifiNMEANode,
    NULL
};
DataModelNode nmeaNode("nmea", &controllerIDNode, nmeaNodeChildren);

static etl::string<maxSessionDescriptionLength> mqttSession1Buffer;
DataModelStringLeaf mqttSession1("1", &mqttSessionsNode, mqttSession1Buffer);
static etl::string<maxSessionDescriptionLength> mqttSession2Buffer;
DataModelStringLeaf mqttSession2("2", &mqttSessionsNode, mqttSession2Buffer);
static etl::string<maxSessionDescriptionLength> mqttSession3Buffer;
DataModelStringLeaf mqttSession3("3", &mqttSessionsNode, mqttSession3Buffer);
static etl::string<maxSessionDescriptionLength> mqttSession4Buffer;
DataModelStringLeaf mqttSession4("4", &mqttSessionsNode, mqttSession4Buffer);
static etl::string<maxSessionDescriptionLength> mqttSession5Buffer;
DataModelStringLeaf mqttSession5("5", &mqttSessionsNode, mqttSession5Buffer);

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

static etl::string<maxConnectionDescriptionLength> mqttConnection1Buffer;
DataModelStringLeaf mqttConnection1("1", &mqttConnectionsNode, mqttConnection1Buffer);
static etl::string<maxConnectionDescriptionLength> mqttConnection2Buffer;
DataModelStringLeaf mqttConnection2("2", &mqttConnectionsNode, mqttConnection2Buffer);
static etl::string<maxConnectionDescriptionLength> mqttConnection3Buffer;
DataModelStringLeaf mqttConnection3("3", &mqttConnectionsNode, mqttConnection3Buffer);
static etl::string<maxConnectionDescriptionLength> mqttConnection4Buffer;
DataModelStringLeaf mqttConnection4("4", &mqttConnectionsNode, mqttConnection4Buffer);
static etl::string<maxConnectionDescriptionLength> mqttConnection5Buffer;
DataModelStringLeaf mqttConnection5("5", &mqttConnectionsNode, mqttConnection5Buffer);

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

DataModelUInt32Leaf mqttConnectionCount("connectionCount", &mqttNode);

DataModelElement *mqttNodeChildren[] = {
    &mqttConnectionCount,
    &mqttConnectionsNode,
    &mqttSessionsNode,
    NULL
};
DataModelNode mqttNode("MQTT", &controllerIDNode, mqttNodeChildren);

static etl::string<maxErrorLength> error1Buffer;
DataModelStringLeaf error1("1", &errorsNode, error1Buffer);
static etl::string<maxErrorLength> error2Buffer;
DataModelStringLeaf error2("2", &errorsNode, error2Buffer);
static etl::string<maxErrorLength> error3Buffer;
DataModelStringLeaf error3("3", &errorsNode, error3Buffer);
static etl::string<maxErrorLength> error4Buffer;
DataModelStringLeaf error4("4", &errorsNode, error4Buffer);
static etl::string<maxErrorLength> error5Buffer;
DataModelStringLeaf error5("5", &errorsNode, error5Buffer);

DataModelStringLeaf *errorDebugs[errorDebugSlots] = {
    &error1,
    &error2,
    &error3,
    &error4,
    &error5
};

DataModelElement *errorsNodeChildren[] = {
    &error1,
    &error2,
    &error3,
    &error4,
    &error5,
    NULL
};
DataModelNode errorsNode("errors", &controllerIDNode, errorsNodeChildren);

DataModelElement *controllerIDNodeChildren[] = {
    &errorsNode,
    &mqttNode,
    &nmeaNode,
    &dataModelNode,
    &nmeaDataModelBridgeNode,
    NULL
};
DataModelNode controllerIDNode(controllerID, &controllersNode, controllerIDNodeChildren);

DataModelElement *controllersNodeChildren[] = { &controllerIDNode, NULL };
DataModelNode controllersNode("controllers", &electronicsNode, controllersNodeChildren);

DataModelElement *electronicsNodeChildren[] = { &controllersNode, NULL };
DataModelNode electronicsNode("electronics", &dataModelRoot, electronicsNodeChildren);

etl::string<coordinateLength> positionLatitudeBuffer;
DataModelStringLeaf positionLatitude("latitude", &positionNode, positionLatitudeBuffer);
etl::string<coordinateLength> positionLongitudeBuffer;
DataModelStringLeaf positionLongitude("longitude", &positionNode, positionLongitudeBuffer);
etl::string<timeLength> positionTimeBuffer;
DataModelStringLeaf positionTime("time", &positionNode, positionTimeBuffer);
DataModelUInt32Leaf positionDataValid("dataValid", &positionNode);
etl::string<15> faaModeIndicatorBuffer;
DataModelStringLeaf positionFAAModeindicator("faaModeIndicator", &positionNode,
                                             faaModeIndicatorBuffer);
DataModelTenthsUInt16Leaf positionSpeedOverGround("speedOverGround", &positionNode);
DataModelTenthsUInt16Leaf positionCourseOverGround("courseOverGround", &positionNode);
DataModelTenthsUInt16Leaf positionTrackMadeGood("trackMadeGood", &positionNode);
etl::string<dateLength> positionDateBuffer;
DataModelStringLeaf positionDate("date", &positionNode, positionDateBuffer);
DataModelTenthsInt16Leaf positionMagneticVariation("magneticVariation", &positionNode);
etl::string<20> positionGPSQualityBuffer;
DataModelStringLeaf positionGPSQuality("gpsQuality", &positionNode, positionGPSQualityBuffer);
DataModelUInt16Leaf positionNumberSatellites("numberSatellites", &positionNode);
DataModelHundredthsUInt16Leaf positionHorizontalDilutionOfPrecision("horizontalDilutionOfPrecision",
                                                                    &positionNode);
DataModelTenthsInt16Leaf positionAntennaAltitude("antennaAltitude", &positionNode);
DataModelTenthsInt16Leaf positionGeoidalSeparation("geoidalSeparation", &positionNode);
DataModelTenthsUInt16Leaf positionGPSDataAge("gpsDataAge", &positionNode);
DataModelUInt16Leaf positionDifferentialReferenceStation("differentialReferenceStation",
                                                         &positionNode);
etl::string<9> positionSatelliteSelectionModeBuffer;
DataModelStringLeaf positionSatelliteSelectionMode("satelliteSelectionMode", &positionNode,
                                                   positionSatelliteSelectionModeBuffer);
etl::string<4> positionFixModeBuffer;
DataModelStringLeaf positionFixMode("fixMode", &positionNode, positionFixModeBuffer);
etl::string<activeSatellitesLength> positionActiveSatellitesBuffer;
DataModelStringLeaf positionActiveSatellites("activeSatellites", &positionNode,
                                             positionActiveSatellitesBuffer);
DataModelHundredthsUInt8Leaf positionPDOP("pdop", &positionNode);
DataModelHundredthsUInt8Leaf positionHDOP("hdop", &positionNode);
DataModelHundredthsUInt8Leaf positionVDOP("vdop", &positionNode);
DataModelTenthsUInt16Leaf
    positionStandardDeviationOfRangeInputsRMS("standardDeviationOfRangeInputsRMS", &positionNode);
DataModelTenthsUInt16Leaf
    positionStandardDeviationOfSemiMajorAxis("standardDeviationOfSemiMajorAxis", &positionNode);
DataModelTenthsUInt16Leaf
    positionStandardDeviationOfSemiMinorAxis("standardDeviationOfSemiMinorAxis", &positionNode);
DataModelTenthsUInt16Leaf positionOrientationOfSemiMajorAxis("orientationOfSemiMajorAxis",
                                                             &positionNode);
DataModelTenthsUInt16Leaf
    positionStandardDeviationOfLatitudeError("standardDeviationOfLatitudeError", &positionNode);
DataModelTenthsUInt16Leaf
    positionStandardDeviationOfLongitudeError("standardDeviationOfLongitudeError", &positionNode);
DataModelTenthsUInt16Leaf
    positionStandardDeviationOfAltitudeError("standardDeviationOfAltitudeError", &positionNode);

DataModelElement *positionNodeChildren[] = {
    &positionLatitude,
    &positionLongitude,
    &positionTime,
    &positionDataValid,
    &positionFAAModeindicator,
    &positionSpeedOverGround,
    &positionCourseOverGround,
    &positionTrackMadeGood,
    &positionDate,
    &positionMagneticVariation,
    &positionGPSQuality,
    &positionNumberSatellites,
    &positionHorizontalDilutionOfPrecision,
    &positionAntennaAltitude,
    &positionGeoidalSeparation,
    &positionGPSDataAge,
    &positionDifferentialReferenceStation,
    &positionSatelliteSelectionMode,
    &positionFixMode,
    &positionActiveSatellites,
    &positionPDOP,
    &positionHDOP,
    &positionVDOP,
    &positionStandardDeviationOfRangeInputsRMS,
    &positionStandardDeviationOfSemiMajorAxis,
    &positionStandardDeviationOfSemiMinorAxis,
    &positionOrientationOfSemiMajorAxis,
    &positionStandardDeviationOfLatitudeError,
    &positionStandardDeviationOfLongitudeError,
    &positionStandardDeviationOfAltitudeError,
    NULL
};
DataModelNode positionNode("position", &navigationNode, positionNodeChildren);

DataModelElement *navigationNodeChildren[] = { &positionNode, NULL };
DataModelNode navigationNode("navigation", &dataModelRoot, navigationNodeChildren);

DataModelElement *topNodeChildren[] = {
    &sysNode,
    &electronicsNode,
    &navigationNode,
    NULL
};
DataModelRoot dataModelRoot(topNodeChildren);

DataModel::DataModel(StatsManager &statsManager) : root(dataModelRoot), leafUpdatesCounter() {
    statsManager.addStatsHolder(this);
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

void DataModel::leafUpdated() {
    leafUpdatesCounter++;
}

void DataModel::exportStats(uint32_t msElapsed) {
    leafUpdatesCounter.update(dataModelLeafUpdates, dataModelLeafUpdateRate, msElapsed);
}
