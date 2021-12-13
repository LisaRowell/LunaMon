#ifndef DATA_MODEL_SUBSCRIBER_H
#define DATA_MODEL_SUBSCRIBER_H

class DataModelSubscriber {
    public:
        virtual void publish(const char *topic, const char *value, bool retainedValue) = 0;
};

#endif