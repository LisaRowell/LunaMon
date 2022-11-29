#include "NMEAGSVSatelitteInfo.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAUInt16.h"
#include "NMEAInt8.h"
#include "NMEATenthsUInt16.h"
#include "NMEAUInt8.h"

#include "Util/Logger.h"

bool NMEAGSVSatelitteInfo::extract(NMEALine &nmeaLine, NMEATalker &talker, bool &endOfInput) {
    if (nmeaLine.atEndOfLine()) {
        endOfInput = true;
        return true;
    }

    if (!id.extract(nmeaLine, talker, "GSV", "Satelitte ID")) {
        return false;
    }

    if (!elevation.extract(nmeaLine, talker, "GSV", "Elevation", false, -90, 90)) {
        return false;
    }

    if (!azimuth.extract(nmeaLine, talker, "GSV", "Azimuth")) {
        return false;
    }

    if (!signalToNoiseRatio.extract(nmeaLine, talker, "GSV", "Signal to Noise Ratio", true, 99)) {
        return false;
    }

    endOfInput = false;
    return true;
}

void NMEAGSVSatelitteInfo::log(Logger &logger) const {
    logger << "ID " << id << " Elevation " << elevation << "\xC2\xB0 Azimuth " << azimuth << " SNR "
           << signalToNoiseRatio << "db";
}
