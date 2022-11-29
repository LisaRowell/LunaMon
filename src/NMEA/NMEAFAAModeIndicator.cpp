#include "NMEAFAAModeIndicator.h"

#include "DataModel/DataModelLeaf.h"

#include "Util/Logger.h"

#include <etl/string_view.h>

NMEAFAAModeIndicator::NMEAFAAModeIndicator() : faaMode(FAA_MODE_NONE) {
}

bool NMEAFAAModeIndicator::set(etl::string_view &faaModeView) {
    if (faaModeView.size() == 0) {
        faaMode = FAA_MODE_NONE;
        return true;
    }

    if (faaModeView.size() > 1) {
        return false;
    }

    const char faaModeChar = faaModeView.front();
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

bool NMEAFAAModeIndicator::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view faaModeIndicatorView;
    if (nmeaLine.getWord(faaModeIndicatorView)) {
        if (!set(faaModeIndicatorView)) {
            logger << logWarning << talker << " " << msgType
                   << " message with bad FAA Mode Indicator field '" << faaModeIndicatorView << "'"
                   << eol;
            return false;
        }
    }

    return true;
}

bool NMEAFAAModeIndicator::hasValue() const {
    return faaMode != FAA_MODE_NONE;
}

void NMEAFAAModeIndicator::publish(DataModelStringLeaf &leaf) const {
    switch (faaMode) {
        case FAA_MODE_NONE:
            break;

        case FAA_MODE_AUTONOMOUS:
            leaf = "Autonomous";
            break;

        case FAA_MODE_CAUTION:
            leaf = "Caution";
            break;

        case FAA_MODE_DIFFERENTIAL:
            leaf = "Differential";
            break;

        case FAA_MODE_ESTIMATED:
            leaf = "Estimated";
            break;

        case FAA_MODE_RTK_FLOAT:
            leaf = "RTK Float";
            break;

        case FAA_MODE_MANUAL:
            leaf = "Manual";
            break;

        case FAA_MODE_DATA_NOT_VALID:
            leaf = "Data Not Valid";
            break;

        case FAA_MODE_PRECISE:
            leaf = "Precise";
            break;

        case FAA_MODE_RTK_INTEGER:
            leaf = "RTK Integer";
            break;

        case FAA_MODE_SIMULATED:
            leaf = "Simulated";
            break;

        case FAA_MODE_UNSAFE:
            leaf = "Unsafe";
    }
}

void NMEAFAAModeIndicator::log(Logger &logger) const {
    switch (faaMode) {
        case FAA_MODE_NONE:
            break;

        case FAA_MODE_AUTONOMOUS:
            logger << "Autonomous";
            break;

        case FAA_MODE_CAUTION:
            logger << "Caution";
            break;

        case FAA_MODE_DIFFERENTIAL:
            logger << "Differential";
            break;

        case FAA_MODE_ESTIMATED:
            logger << "Estimated";
            break;

        case FAA_MODE_RTK_FLOAT:
            logger << "RTK Float";
            break;

        case FAA_MODE_MANUAL:
            logger << "Manual";
            break;

        case FAA_MODE_DATA_NOT_VALID:
            logger << "Data Not Valid";
            break;

        case FAA_MODE_PRECISE:
            logger << "Precise";
            break;

        case FAA_MODE_RTK_INTEGER:
            logger << "RTK Integer";
            break;

        case FAA_MODE_SIMULATED:
            logger << "Simulated";
            break;

        case FAA_MODE_UNSAFE:
            logger << "Unsafe";
    }
}
