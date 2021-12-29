#ifndef NMEA_FAA_MODE_INDICATOR_H
#define NMEA_FAA_MODE_INDICATOR_H

#include "DataModel/DataModelLeaf.h"

class NMEAFAAModeIndicator {
    private:
        enum FAAMode {
            FAA_MODE_NONE,
            FAA_MODE_AUTONOMOUS,
            FAA_MODE_CAUTION,
            FAA_MODE_DIFFERENTIAL,
            FAA_MODE_ESTIMATED,
            FAA_MODE_RTK_FLOAT,
            FAA_MODE_MANUAL,
            FAA_MODE_DATA_NOT_VALID,
            FAA_MODE_PRECISE,
            FAA_MODE_RTK_INTEGER,
            FAA_MODE_SIMULATED,
            FAA_MODE_UNSAFE
        };
        FAAMode faaMode;

    public:
        NMEAFAAModeIndicator();
        bool set(String &faaModeStr);
        bool hasValue() const;
        void print() const;
        void publish(DataModelLeaf &leaf) const;
};

#endif