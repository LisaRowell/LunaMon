#ifndef DATA_MODEL_ELEMENT_H
#define DATA_MODEL_ELEMENT_H

#include "DataModelSubscriber.h"

const unsigned maxDataModelSubscribers = 5;

class DataModelElement {
    private:
        const char *name;
        DataModelElement *parent;

    protected:
        bool isMultiLevelWildcard(const char *topicFilter);
        bool topicFilterMatch(const char *topicFilter, unsigned &offsetToNextLevel,
                              bool &lastLevel);
        void buildTopicName(char *topicNameBuffer);

    public:
        DataModelElement(const char *name, DataModelElement *parent);
        // Returns true if one or more subscriptions were made
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) = 0;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) = 0;
};

#endif