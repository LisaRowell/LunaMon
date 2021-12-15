#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include "DataModelNode.h"
#include "DataModelRoot.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelSubscriber.h"

extern DataModelUInt32Leaf mqttSessions;
extern DataModelUInt32Leaf mqttConnections;

extern DataModelNode mqttNode;

extern DataModelLeaf controllerUpTime;
extern DataModelNode controllerIDNode;

extern DataModelNode controllersNode;

extern DataModelNode electronicsNode;

extern DataModelRoot dataModelRoot;

const unsigned maxDataModelClients = 2;

// This is probably in need of consideration...
const unsigned maxTopicNameLength = 255;
const unsigned maxTopicFilterLength = maxTopicNameLength;

const char dataModelLevelSeparator = '/';
const char dataModelMultiLevelWildcard = '#';
const char dataModelSingleLevelWildcard = '+';

class DataModel {
    private:
        DataModelRoot &root;

    public:
        DataModel();
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
};

extern DataModel dataModel;

#endif