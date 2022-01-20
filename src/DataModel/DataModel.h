#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <Arduino.h>

#include "DataModelNode.h"
#include "DataModelRoot.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelStringLeaf.h"
#include "DataModelSubscriber.h"

#include "MQTT/MQTTSession.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsHolder.h"
#include "StatsManager/StatsManager.h"

#include "Util/IPAddressTools.h"

const size_t maxTCPPortTextLength = 5;
const size_t maxConnectionDescriptionLength =
    maxIPAddressTextLength + 1 + maxTCPPortTextLength + 2 + maxMQTTClientIDLength + 1 + 1;

const size_t maxSessionDescriptionLength =
    maxMQTTClientIDLength + 2 + maxIPAddressTextLength + 1 + maxTCPPortTextLength + 1 + 1;

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

extern DataModelUInt32Leaf mqttSessionCount;
extern DataModelUInt32Leaf mqttConnectionCount;

extern DataModelNode mqttNode;

const size_t maxErrorLength = 80;
const unsigned errorDebugSlots = 5;
extern DataModelStringLeaf *errorDebugs[];
extern DataModelNode errorsNode;

extern DataModelUInt32Leaf controllerUpTime;
extern DataModelNode controllerIDNode;

extern DataModelNode controllersNode;

extern DataModelNode electronicsNode;

extern DataModelLeaf positionLatitude;
extern DataModelLeaf positionLongitude;
extern DataModelLeaf positionTime;
extern DataModelLeaf positionDataValid;
extern DataModelLeaf positionFAAModeindicator;
extern DataModelLeaf positionSpeedOverGround;
extern DataModelLeaf positionTrackMadeGood;
extern DataModelLeaf positionDate;
extern DataModelLeaf positionMagneticVariation;
extern DataModelLeaf positionGPSQuality;
extern DataModelLeaf positionNumberSatellites;
extern DataModelLeaf positionHorizontalDilutionOfPrecision;
extern DataModelLeaf positionAntennaAltitude;
extern DataModelLeaf positionGeoidalSeparation;
extern DataModelLeaf positionGPSDataAge;
extern DataModelLeaf positionDifferentialReferenceStation;
extern DataModelLeaf positionSatelliteSelectionMode;
extern DataModelLeaf positionFixMode;
extern DataModelLeaf positionActiveSatellites;
extern DataModelLeaf positionPDOP;
extern DataModelLeaf positionHDOP;
extern DataModelLeaf positionVDOP;
extern DataModelLeaf positionStandardDeviationOfRangeInputsRMS;
extern DataModelLeaf positionStandardDeviationOfSemiMajorAxis;
extern DataModelLeaf positionStandardDeviationOfSemiMinorAxis;
extern DataModelLeaf positionOrientationOfSemiMajorAxis;
extern DataModelLeaf positionStandardDeviationOfLatitudeError;
extern DataModelLeaf positionStandardDeviationOfLongitudeError;
extern DataModelLeaf positionStandardDeviationOfAltitudeError;

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
