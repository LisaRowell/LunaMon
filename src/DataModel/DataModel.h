#ifndef DATA_MODEL_H
#define DATA_MODEL_H

class DataModelSubscriber;
class StatsManager;

#include "DataModelNode.h"
#include "DataModelRoot.h"
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

extern DataModelUInt32Leaf sysBrokerClientsConnected;
extern DataModelUInt32Leaf sysBrokerClientsDisconnected;
extern DataModelUInt32Leaf sysBrokerClientsMaximum;
extern DataModelUInt32Leaf sysBrokerClientsTotal;
extern DataModelNode sysBrokerClientsNode;

extern DataModelUInt32Leaf sysBrokerMessagesPublishReceived;
extern DataModelUInt32Leaf sysBrokerMessagesPublishSent;
extern DataModelUInt32Leaf sysBrokerMessagesPublishDropped;
extern DataModelNode sysBrokerMessagesPublishNode;

extern DataModelUInt32Leaf sysBrokerMessagesReceived;
extern DataModelUInt32Leaf sysBrokerMessagesSent;
extern DataModelNode sysBrokerMessagesNode;

extern DataModelUInt32Leaf sysBrokerUptime;

extern DataModelNode sysBrokerNode;
extern DataModelNode sysNode;

extern DataModelLeaf nmeaDataModelMessagesBridged;
extern DataModelLeaf nmeaDataModelMessageBridgeRate;

extern DataModelNode nmeaDataModelBridgeNode;

extern DataModelLeaf dataModelLeafUpdates;
extern DataModelLeaf dataModelLeafUpdateRate;

extern DataModelNode dataModelNode;

extern DataModelLeaf wifiNMEAMessages;
extern DataModelLeaf wifiNMEAMessageRate;
extern DataModelNode wifiNMEANode;

extern DataModelLeaf usbNMEAMessages;
extern DataModelLeaf usbNMEAMessageRate;
extern DataModelNode usbNMEANode;

extern DataModelNode nmeaNode;

extern DataModelStringLeaf *mqttSessionDebugs[];
extern DataModelNode mqttSessionsNode;

extern DataModelStringLeaf *mqttConnectionDebugs[];
extern DataModelNode mqttConnectionsNode;

extern DataModelUInt32Leaf mqttConnectionCount;

extern DataModelNode mqttNode;

const size_t maxErrorLength = 80;
const unsigned errorDebugSlots = 5;
extern DataModelStringLeaf *errorDebugs[];
extern DataModelNode errorsNode;

extern DataModelNode controllerIDNode;

extern DataModelNode controllersNode;

extern DataModelNode electronicsNode;

constexpr size_t coordinateLength = 40;
extern DataModelStringLeaf positionLatitude;
extern DataModelStringLeaf positionLongitude;
constexpr size_t timeLength = 40;
extern DataModelStringLeaf positionTime;
extern DataModelUInt32Leaf positionDataValid;
extern DataModelStringLeaf positionFAAModeindicator;
extern DataModelTenthsUInt16Leaf positionSpeedOverGround;
extern DataModelTenthsUInt16Leaf positionCourseOverGround;
extern DataModelTenthsUInt16Leaf positionTrackMadeGood;
constexpr size_t dateLength = 10;
extern DataModelStringLeaf positionDate;
extern DataModelTenthsInt16Leaf positionMagneticVariation;
extern DataModelStringLeaf positionGPSQuality;
extern DataModelUInt16Leaf positionNumberSatellites;
extern DataModelHundredthsUInt16Leaf positionHorizontalDilutionOfPrecision;
extern DataModelTenthsInt16Leaf positionAntennaAltitude;
extern DataModelTenthsInt16Leaf positionGeoidalSeparation;
extern DataModelTenthsUInt16Leaf positionGPSDataAge;
extern DataModelUInt16Leaf positionDifferentialReferenceStation;
extern DataModelStringLeaf positionSatelliteSelectionMode;
extern DataModelStringLeaf positionFixMode;
constexpr size_t activeSatellitesLength = 72;
extern DataModelStringLeaf positionActiveSatellites;
extern DataModelHundredthsUInt8Leaf positionPDOP;
extern DataModelHundredthsUInt8Leaf positionHDOP;
extern DataModelHundredthsUInt8Leaf positionVDOP;
extern DataModelTenthsUInt16Leaf positionStandardDeviationOfRangeInputsRMS;
extern DataModelTenthsUInt16Leaf positionStandardDeviationOfSemiMajorAxis;
extern DataModelTenthsUInt16Leaf positionStandardDeviationOfSemiMinorAxis;
extern DataModelTenthsUInt16Leaf positionOrientationOfSemiMajorAxis;
extern DataModelTenthsUInt16Leaf positionStandardDeviationOfLatitudeError;
extern DataModelTenthsUInt16Leaf positionStandardDeviationOfLongitudeError;
extern DataModelTenthsUInt16Leaf positionStandardDeviationOfAltitudeError;

extern DataModelNode positionNode;

extern DataModelNode navigationNode;

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
