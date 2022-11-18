#ifndef DATA_MODEL_SUBSCRIBER_H
#define DATA_MODEL_SUBSCRIBER_H

#include <etl/string.h>

using etl::istring;

class DataModelSubscriber {
    public:
        virtual void publish(const char *topic, const char *value, bool retainedValue) = 0;
        virtual const istring &name() const = 0;
};

#endif
