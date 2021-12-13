#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include "DataModelNode.h"
#include "DataModelRoot.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelSubscriber.h"

extern DataModelUInt32Leaf controllerUpTime;
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
};

extern DataModel dataModel;

#endif