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

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

class DataModelSubscriber;
class StatsManager;

#include "DataModelNode.h"
#include "DataModelRoot.h"
#include "DataModelBoolLeaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"
#include "DataModelStringLeaf.h"

#include "MQTT/MQTTSession.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsHolder.h"

#include "Util/IPAddressTools.h"

#include <stdint.h>

const size_t maxTCPPortTextLength = 5;
const size_t maxConnectionDescriptionLength =
    maxIPAddressTextLength + 1 + maxTCPPortTextLength + 2 + maxMQTTClientIDLength + 1 + 1;

const size_t maxSessionDescriptionLength =
    maxMQTTClientIDLength + 2 + maxIPAddressTextLength + 1 + maxTCPPortTextLength + 1 + 1;

const size_t maxVersionLength = 20;

extern DataModelStringLeaf *sysBrokerConnectionDebugs[];
extern DataModelNode sysBrokerConnectionsNode;

extern DataModelStringLeaf *sysBrokerSessionDebugs[];
extern DataModelNode sysBrokerSessionsNode;

extern DataModelUInt32Leaf sysBrokerClientsConnected;
extern DataModelUInt32Leaf sysBrokerClientsDisconnected;
extern DataModelUInt32Leaf sysBrokerClientsMaximum;
extern DataModelUInt32Leaf sysBrokerClientsTotal;
extern DataModelNode sysBrokerClientsNode;

extern DataModelUInt16Leaf sysBrokerMessagesRetainedCount;
extern DataModelNode sysBrokerMessagesRetainedNode;

extern DataModelUInt32Leaf sysBrokerMessagesPublishReceived;
extern DataModelUInt32Leaf sysBrokerMessagesPublishSent;
extern DataModelUInt32Leaf sysBrokerMessagesPublishDropped;
extern DataModelNode sysBrokerMessagesPublishNode;

extern DataModelUInt32Leaf sysBrokerMessagesReceived;
extern DataModelUInt32Leaf sysBrokerMessagesSent;
extern DataModelNode sysBrokerMessagesNode;

extern DataModelUInt32Leaf sysBrokerSubscriptionsCount;
extern DataModelNode sysBrokerSubscriptionsNode;

extern DataModelUInt32Leaf sysBrokerUptime;
extern DataModelStringLeaf sysBrokerVersion;
extern DataModelNode sysBrokerNode;

extern DataModelBoolLeaf sysNEMAWiFiState;
extern DataModelLeaf sysNMEAWiFiMessages;
extern DataModelLeaf sysNMEAWiFiMessageRate;
extern DataModelNode sysNMEAWiFiNode;

extern DataModelLeaf sysNMEAUSBMessages;
extern DataModelLeaf sysNMEAUSBMessageRate;
extern DataModelNode sysNMEAUSBNode;

extern DataModelNode sysNMEANode;

extern DataModelLeaf sysDataModelLeafUpdates;
extern DataModelLeaf sysDataModelLeafUpdateRate;
extern DataModelNode sysDataModelNode;

extern DataModelLeaf sysNMEADataModelMessagesBridged;
extern DataModelLeaf sysNMEADataModelMessageBridgeRate;
extern DataModelNode sysNMEADataModelBridgeNode;

const size_t maxLogEntryLength = 80;
const unsigned logEntrySlots = 5;
extern DataModelStringLeaf *sysLogEntries[];
extern DataModelNode sysLogNode;

extern DataModelNode sysNode;

constexpr size_t timeLength = 15;
extern DataModelStringLeaf gpsTime;
constexpr size_t dateLength = 10;
extern DataModelStringLeaf gpsDate;
extern DataModelBoolLeaf gpsDataValid;
constexpr size_t coordinateLength = 20;
extern DataModelStringLeaf gpsLatitude;
extern DataModelStringLeaf gpsLongitude;
extern DataModelTenthsInt16Leaf gpsAltitude;
extern DataModelTenthsUInt16Leaf gpsSpeedOverGround;
extern DataModelTenthsUInt16Leaf gpsSpeedOverGroundKmPerH;
extern DataModelTenthsUInt16Leaf gpsTrackMadeGoodTrue;
extern DataModelTenthsUInt16Leaf gpsTrackMadeGoodMagnetic;
extern DataModelTenthsInt16Leaf gpsMagneticVariation;
extern DataModelStringLeaf gpsFAAModeindicator;
extern DataModelStringLeaf gpsGPSQuality;
extern DataModelUInt16Leaf gpsNumberSatellites;
extern DataModelHundredthsUInt16Leaf gpsHorizontalDilutionOfPrecision;
extern DataModelTenthsInt16Leaf gpsGeoidalSeparation;
extern DataModelTenthsUInt16Leaf gpsDataAge;
extern DataModelUInt16Leaf gpsDifferentialReferenceStation;
extern DataModelStringLeaf gpsSatelliteSelectionMode;
extern DataModelStringLeaf gpsFixMode;
constexpr size_t activeSatellitesLength = 72;
extern DataModelStringLeaf gpsActiveSatellites;
extern DataModelHundredthsUInt8Leaf gpsPDOP;
extern DataModelHundredthsUInt8Leaf gpsHDOP;
extern DataModelHundredthsUInt8Leaf gpsVDOP;
extern DataModelTenthsUInt16Leaf gpsStandardDeviationOfRangeInputsRMS;
extern DataModelTenthsUInt16Leaf gpsStandardDeviationOfSemiMajorAxis;
extern DataModelTenthsUInt16Leaf gpsStandardDeviationOfSemiMinorAxis;
extern DataModelTenthsUInt16Leaf gpsOrientationOfSemiMajorAxis;
extern DataModelTenthsUInt16Leaf gpsStandardDeviationOfLatitudeError;
extern DataModelTenthsUInt16Leaf gpsStandardDeviationOfLongitudeError;
extern DataModelTenthsUInt16Leaf gpsStandardDeviationOfAltitudeError;
extern DataModelNode gpsNode;

extern DataModelTenthsUInt16Leaf depthBelowTransducerFeet;
extern DataModelTenthsUInt16Leaf depthBelowTransducerMeters;
extern DataModelTenthsUInt16Leaf depthBelowTransducerFathoms;
extern DataModelNode depthBelowTransducerNode;

extern DataModelTenthsUInt16Leaf depthBelowKeelFeet;
extern DataModelTenthsUInt16Leaf depthBelowKeelMeters;
extern DataModelTenthsUInt16Leaf depthBelowKeelFathoms;
extern DataModelNode depthBelowKeelNode;

extern DataModelTenthsUInt16Leaf depthBelowSurfaceFeet;
extern DataModelTenthsUInt16Leaf depthBelowSurfaceMeters;
extern DataModelTenthsUInt16Leaf depthBelowSurfaceFathoms;
extern DataModelNode depthBelowSurfaceNode;

extern DataModelNode depthNode;

extern DataModelRoot dataModelRoot;

const unsigned maxDataModelClients = 2;

// This is probably in need of consideration...
const unsigned maxTopicNameLength = 255;
const unsigned maxTopicFilterLength = maxTopicNameLength;

const char dataModelLevelSeparator = '/';
const char dataModelMultiLevelWildcard = '#';
const char dataModelSingleLevelWildcard = '+';

class DataModel : public StatsHolder {
    private:
        DataModelRoot &root;
        StatCounter leafUpdatesCounter;

    public:
        DataModel(StatsManager &statsManager);
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber);
        // This method made need revisiting in the future. Currently we don't store references to
        // what topics an MQTT Session is subscribed to, and the only way we can unsubscribe from
        // all of its topics is to search the tree, finding and removing its subscriptions. Since
        // we are running in an Arduino-esq, proto-threaded environment, this could lead to serial
        // overruns if the tree grew dramatically in complexity. The alternative is to cache
        // subscribed to topics at a Session level, but this is problematic because of the potential
        // number of topics a client may be interested in and the limited dynamic memory of the
        // Arduino syystems we target. At some poiint this should be metered to see how bad it is.
        // Fortunately, NMEA 0183 is limited to fairly low bandwidths...
        void unsubscribeAll(DataModelSubscriber &subscriber);
        void leafUpdated();
        virtual void exportStats(uint32_t msElapsed) override;
};

extern DataModel dataModel;

#endif
