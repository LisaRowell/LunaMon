#include <Arduino.h>

#include "NMEAFAAModeIndicator.h"

#include "DataModel/DataModelLeaf.h"

NMEAFAAModeIndicator::NMEAFAAModeIndicator() : faaMode(FAA_MODE_NONE) {
}

bool NMEAFAAModeIndicator::set(String &faaModeStr) {
    if (faaModeStr.length() == 0) {
        faaMode = FAA_MODE_NONE;
        return true;
    }

    if (faaModeStr.length() > 1) {
        return false;
    }

    char faaModeChar = faaModeStr.charAt(0);
    switch (faaModeChar) {
        case 'A':
            faaMode = FAA_MODE_AUTONOMOUS;
            return true;

        case 'C':
            faaMode = FAA_MODE_CAUTION;
            return true;

        case 'D':
            faaMode = FAA_MODE_DIFFERENTIAL;
            return true;

        case 'E':
            faaMode = FAA_MODE_ESTIMATED;
            return true;

        case 'F':
            faaMode = FAA_MODE_RTK_FLOAT;
            return true;

        case 'M':
            faaMode = FAA_MODE_MANUAL;
            return true;

        case 'N':
            faaMode = FAA_MODE_DATA_NOT_VALID;
            return true;

        case 'P':
            faaMode = FAA_MODE_PRECISE;
            return true;

        case 'R':
            faaMode = FAA_MODE_RTK_INTEGER;
            return true;

        case 'S':
            faaMode = FAA_MODE_SIMULATED;
            return true;

        case 'U':
            faaMode = FAA_MODE_UNSAFE;
            return true;

        default:
            return false;
    }
}

bool NMEAFAAModeIndicator::hasValue() const {
    return faaMode != FAA_MODE_NONE;
}

void NMEAFAAModeIndicator::print() const {
    switch (faaMode) {
        case FAA_MODE_NONE:
            break;

        case FAA_MODE_AUTONOMOUS:
            Serial.print("Autonomous");
            break;

        case FAA_MODE_CAUTION:
            Serial.print("Caution");
            break;

        case FAA_MODE_DIFFERENTIAL:
            Serial.print("Differential");
            break;

        case FAA_MODE_ESTIMATED:
            Serial.print("Estimated");
            break;

        case FAA_MODE_RTK_FLOAT:
            Serial.print("RTK Float");
            break;

        case FAA_MODE_MANUAL:
            Serial.print("Manual");
            break;

        case FAA_MODE_DATA_NOT_VALID:
            Serial.print("Data Not Valid");
            break;

        case FAA_MODE_PRECISE:
            Serial.print("Precise");
            break;

        case FAA_MODE_RTK_INTEGER:
            Serial.print("RTK Integer");
            break;

        case FAA_MODE_SIMULATED:
            Serial.print("Simulated");
            break;

        case FAA_MODE_UNSAFE:
            Serial.print("Unsafe");
    }
}

void NMEAFAAModeIndicator::publish(DataModelLeaf &leaf) const {
    switch (faaMode) {
        case FAA_MODE_NONE:
            break;

        case FAA_MODE_AUTONOMOUS:
            leaf << "Autonomous";
            break;

        case FAA_MODE_CAUTION:
            leaf << "Caution";
            break;

        case FAA_MODE_DIFFERENTIAL:
            leaf << "Differential";
            break;

        case FAA_MODE_ESTIMATED:
            leaf << "Estimated";
            break;

        case FAA_MODE_RTK_FLOAT:
            leaf << "RTK Float";
            break;

        case FAA_MODE_MANUAL:
            leaf << "Manual";
            break;

        case FAA_MODE_DATA_NOT_VALID:
            leaf << "Data Not Valid";
            break;

        case FAA_MODE_PRECISE:
            leaf << "Precise";
            break;

        case FAA_MODE_RTK_INTEGER:
            leaf << "RTK Integer";
            break;

        case FAA_MODE_SIMULATED:
            leaf << "Simulated";
            break;

        case FAA_MODE_UNSAFE:
            leaf << "Unsafe";
    }
}
