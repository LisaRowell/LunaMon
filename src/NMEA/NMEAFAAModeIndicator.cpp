#include <Arduino.h>

#include "NMEAFAAModeIndicator.h"

bool parseNMEAFAAModeIndicator(String &faaModeStr, enum NMEAFAAModeIndicator &faaModeIndicator) {
    if (faaModeStr.length() == 0) {
        faaModeIndicator = FAA_MODE_NONE;
        return true;
    }

    if (faaModeStr.length() > 1) {
        return false;
    }

    char faaModeChar = faaModeStr.charAt(0);
    switch (faaModeChar) {
        case 'A':
            faaModeIndicator = FAA_MODE_AUTONOMOUS;
            return true;

        case 'D':
            faaModeIndicator = FAA_MODE_DIFFERENTIAL;
            return true;

        case 'E':
            faaModeIndicator = FAA_MODE_ESTIMATED;
            return true;

        case 'F':
            faaModeIndicator = FAA_MODE_RTK_FLOAT;
            return true;

        case 'M':
            faaModeIndicator = FAA_MODE_MANUAL;
            return true;

        case 'N':
            faaModeIndicator = FAA_MODE_DATA_NOT_VALID;
            return true;

        case 'P':
            faaModeIndicator = FAA_MODE_PRECISE;
            return true;

        case 'R':
            faaModeIndicator = FAA_MODE_RTK_INTEGER;
            return true;

        case 'S':
            faaModeIndicator = FAA_MODE_SIMULATED;
            return true;

        default:
            return false;
    }
}

void printNMEAFAAModeIndicator(enum NMEAFAAModeIndicator faaModeIndicator) {
    switch (faaModeIndicator) {
        case FAA_MODE_NONE:
            break;

        case FAA_MODE_AUTONOMOUS:
            Serial.print("Autonomous");
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
    }
}