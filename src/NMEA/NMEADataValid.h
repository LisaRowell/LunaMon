#ifndef NMEA_DATA_VALID_H
#define NMEA_DATA_VALID_H

#include "DataModel/DataModelLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

class NMEADataValid : public LoggableItem {
    private:
        bool valid;

    public:
        NMEADataValid();
        bool set(String &validStr);
        void publish(DataModelLeaf &leaf);
        virtual void log(Logger &logger) const override;
};

#endif
