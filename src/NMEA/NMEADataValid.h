#ifndef NMEA_DATA_VALID_H
#define NMEA_DATA_VALID_H

#include "DataModel/DataModelLeaf.h"

class NMEADataValid {
    private:
        bool valid;

    public:
        NMEADataValid();
        bool set(String &validStr);
        void print();
        void publish(DataModelLeaf &leaf);
};

#endif
