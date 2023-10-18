/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelRoot.h"
#include "DataModelBoolLeaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"
#include "DataModelStringLeaf.h"
#include "Config.h"
#include "Version.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsManager.h"

#include "Util/Logger.h"
#include "Util/Error.h"

#include <etl/string.h>

#include <stdint.h>

static etl::string<maxConnectionDescriptionLength> sysBrokerConnection1Buffer;
DataModelStringLeaf sysBrokerConnection1("1", &sysBrokerConnectionsNode,
                                         sysBrokerConnection1Buffer);
static etl::string<maxConnectionDescriptionLength> sysBrokerConnection2Buffer;
DataModelStringLeaf sysBrokerConnection2("2", &sysBrokerConnectionsNode,
                                         sysBrokerConnection2Buffer);
static etl::string<maxConnectionDescriptionLength> sysBrokerConnection3Buffer;
DataModelStringLeaf sysBrokerConnection3("3", &sysBrokerConnectionsNode,
                                         sysBrokerConnection3Buffer);
static etl::string<maxConnectionDescriptionLength> sysBrokerConnection4Buffer;
DataModelStringLeaf sysBrokerConnection4("4", &sysBrokerConnectionsNode,
                                         sysBrokerConnection4Buffer);
static etl::string<maxConnectionDescriptionLength> sysBrokerConnection5Buffer;
DataModelStringLeaf sysBrokerConnection5("5", &sysBrokerConnectionsNode,
                                         sysBrokerConnection5Buffer);

DataModelStringLeaf *sysBrokerConnectionDebugs[] = {
    &sysBrokerConnection1,
    &sysBrokerConnection2,
    &sysBrokerConnection3,
    &sysBrokerConnection4,
    &sysBrokerConnection5
};

DataModelElement *sysBrokerConnectionsNodeChildren[] = {
    &sysBrokerConnection1,
    &sysBrokerConnection2,
    &sysBrokerConnection3,
    &sysBrokerConnection4,
    &sysBrokerConnection5,
    NULL
};
DataModelNode sysBrokerConnectionsNode("connections", &sysBrokerNode,
                                       sysBrokerConnectionsNodeChildren);

static etl::string<maxSessionDescriptionLength> sysBrokerSession1Buffer;
DataModelStringLeaf sysBrokerSession1("1", &sysBrokerSessionsNode, sysBrokerSession1Buffer);
static etl::string<maxSessionDescriptionLength> sysBrokerSession2Buffer;
DataModelStringLeaf sysBrokerSession2("2", &sysBrokerSessionsNode, sysBrokerSession2Buffer);
static etl::string<maxSessionDescriptionLength> sysBrokerSession3Buffer;
DataModelStringLeaf sysBrokerSession3("3", &sysBrokerSessionsNode, sysBrokerSession3Buffer);
static etl::string<maxSessionDescriptionLength> sysBrokerSession4Buffer;
DataModelStringLeaf sysBrokerSession4("4", &sysBrokerSessionsNode, sysBrokerSession4Buffer);
static etl::string<maxSessionDescriptionLength> sysBrokerSession5Buffer;
DataModelStringLeaf sysBrokerSession5("5", &sysBrokerSessionsNode, sysBrokerSession5Buffer);

DataModelStringLeaf *sysBrokerSessionDebugs[] = {
    &sysBrokerSession1,
    &sysBrokerSession2,
    &sysBrokerSession3,
    &sysBrokerSession4,
    &sysBrokerSession5
};

DataModelElement *sysBrokerSessionsNodeChildren[] = {
    &sysBrokerSession1,
    &sysBrokerSession2,
    &sysBrokerSession3,
    &sysBrokerSession4,
    &sysBrokerSession5,
    NULL
};
DataModelNode sysBrokerSessionsNode("sessions", &sysBrokerNode, sysBrokerSessionsNodeChildren);

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

DataModelUInt16Leaf sysBrokerMessagesRetainedCount("count", &sysBrokerMessagesRetainedNode);

DataModelElement *sysBrokerMessagesRetainedChildren[] = {
    &sysBrokerMessagesRetainedCount,
    NULL
};
DataModelNode sysBrokerMessagesRetainedNode("retained", &sysBrokerMessagesNode,
                                            sysBrokerMessagesRetainedChildren);

DataModelUInt32Leaf sysBrokerMessagesPublishReceived("received", &sysBrokerMessagesPublishNode);
DataModelUInt32Leaf sysBrokerMessagesPublishSent("sent", &sysBrokerMessagesPublishNode);
DataModelUInt32Leaf sysBrokerMessagesPublishDropped("dropped", &sysBrokerMessagesPublishNode);


DataModelElement *sysBrokerMessagesPublishChildren[] = {
    &sysBrokerMessagesPublishReceived,
    &sysBrokerMessagesPublishSent,
    &sysBrokerMessagesPublishDropped,
    NULL
};
DataModelNode sysBrokerMessagesPublishNode("publish", &sysBrokerMessagesNode,
                                           sysBrokerMessagesPublishChildren);

DataModelUInt32Leaf sysBrokerMessagesReceived("received", &sysBrokerMessagesNode);
DataModelUInt32Leaf sysBrokerMessagesSent("sent", &sysBrokerMessagesNode);

DataModelElement *sysBrokerMessagesChildren[] = {
    &sysBrokerMessagesReceived,
    &sysBrokerMessagesSent,
    &sysBrokerMessagesPublishNode,
    &sysBrokerMessagesRetainedNode,
    NULL
};
DataModelNode sysBrokerMessagesNode("messages", &sysBrokerNode, sysBrokerMessagesChildren);

DataModelUInt32Leaf sysBrokerSubscriptionsCount("count", &sysBrokerSubscriptionsNode);

DataModelElement *sysBrokerSubscriptionsChildren[] = {
    &sysBrokerSubscriptionsCount,
    NULL
};
DataModelNode sysBrokerSubscriptionsNode("subscriptions", &sysBrokerNode,
                                         sysBrokerSubscriptionsChildren);

DataModelUInt32Leaf sysBrokerUptime("uptime", &sysBrokerNode);
static etl::string<maxVersionLength> sysBrokerVersionBuffer;
DataModelStringLeaf sysBrokerVersion("version", &sysBrokerNode, sysBrokerVersionBuffer);

DataModelElement *sysBrokerChildren[] = {
    &sysBrokerConnectionsNode,
    &sysBrokerSessionsNode,
    &sysBrokerClientsNode,
    &sysBrokerMessagesNode,
    &sysBrokerSubscriptionsNode,
    &sysBrokerUptime,
    &sysBrokerVersion,
    NULL
};
DataModelNode sysBrokerNode("broker", &sysNode, sysBrokerChildren);

DataModelBoolLeaf sysNEMAWiFiState("state", &sysNMEAWiFiNode);
DataModelLeaf sysNMEAWiFiMessages("messages", &sysNMEAWiFiNode);
DataModelLeaf sysNMEAWiFiMessageRate("messageRate", &sysNMEAWiFiNode);

DataModelElement *sysNMEAWiFiNodeChildren[] = {
    &sysNEMAWiFiState,
    &sysNMEAWiFiMessages,
    &sysNMEAWiFiMessageRate,
    NULL
};
DataModelNode sysNMEAWiFiNode("wifi", &sysNMEANode, sysNMEAWiFiNodeChildren);

DataModelLeaf sysNMEAUSBMessages("messages", &sysNMEAUSBNode);
DataModelLeaf sysNMEAUSBMessageRate("messageRate", &sysNMEAUSBNode);

DataModelElement *sysNMEAUSBNodeChildren[] = {
    &sysNMEAUSBMessages,
    &sysNMEAUSBMessageRate,
    NULL
};
DataModelNode sysNMEAUSBNode("usb", &sysNMEANode, sysNMEAUSBNodeChildren);

DataModelElement *sysNMEANodeChildren[] = {
    &sysNMEAUSBNode,
    &sysNMEAWiFiNode,
    NULL
};
DataModelNode sysNMEANode("nmea", &sysNode, sysNMEANodeChildren);

DataModelLeaf sysDataModelLeafUpdates("updates", &sysDataModelNode);
DataModelLeaf sysDataModelLeafUpdateRate("updateRate", &sysDataModelNode);

DataModelElement *sysDataModelNodeChildren[] = {
    &sysDataModelLeafUpdates,
    &sysDataModelLeafUpdateRate,
    NULL
};
DataModelNode sysDataModelNode("dataModel", &sysNode, sysDataModelNodeChildren);

DataModelLeaf sysNMEADataModelMessagesBridged("messages", &sysNMEADataModelBridgeNode);
DataModelLeaf sysNMEADataModelMessageBridgeRate("messageRate", &sysNMEADataModelBridgeNode);

DataModelElement *sysNMEADataModelBridgeNodeChildren[] = {
    &sysNMEADataModelMessagesBridged,
    &sysNMEADataModelMessageBridgeRate,
    NULL
};
DataModelNode sysNMEADataModelBridgeNode("nmeaDataModelBridge", &sysNode,
                                         sysNMEADataModelBridgeNodeChildren);

static etl::string<maxLogEntryLength> sysLogEntry1Buffer;
DataModelStringLeaf sysLogEntry1("1", &sysLogNode, sysLogEntry1Buffer);
static etl::string<maxLogEntryLength> sysLogEntry2Buffer;
DataModelStringLeaf sysLogEntry2("2", &sysLogNode, sysLogEntry2Buffer);
static etl::string<maxLogEntryLength> sysLogEntry3Buffer;
DataModelStringLeaf sysLogEntry3("3", &sysLogNode, sysLogEntry3Buffer);
static etl::string<maxLogEntryLength> sysLogEntry4Buffer;
DataModelStringLeaf sysLogEntry4("4", &sysLogNode, sysLogEntry4Buffer);
static etl::string<maxLogEntryLength> sysLogEntry5Buffer;
DataModelStringLeaf sysLogEntry5("5", &sysLogNode, sysLogEntry5Buffer);

DataModelStringLeaf *sysLogEntries[logEntrySlots] = {
    &sysLogEntry1,
    &sysLogEntry2,
    &sysLogEntry3,
    &sysLogEntry4,
    &sysLogEntry5
};

DataModelElement *sysLogNodeChildren[] = {
    &sysLogEntry1,
    &sysLogEntry2,
    &sysLogEntry3,
    &sysLogEntry4,
    &sysLogEntry5,
    NULL
};
DataModelNode sysLogNode("log", &sysNode, sysLogNodeChildren);

DataModelElement *sysNodeChildren[] = {
    &sysBrokerNode,
    &sysNMEANode,
    &sysDataModelNode,
    &sysNMEADataModelBridgeNode,
    &sysLogNode,
    NULL
};
DataModelNode sysNode("$SYS", &dataModelRoot, sysNodeChildren);

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
    &navigationNode,
    NULL
};
DataModelRoot dataModelRoot(topNodeChildren);

DataModel::DataModel(StatsManager &statsManager) : root(dataModelRoot), leafUpdatesCounter() {
    statsManager.addStatsHolder(this);

    sysBrokerVersion = VERSION;
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
    leafUpdatesCounter.update(sysDataModelLeafUpdates, sysDataModelLeafUpdateRate, msElapsed);

    sysBrokerMessagesRetainedCount = DataModelRetainedValueLeaf::retainedValueCount();
}
