#include <Arduino.h>

#include "NMEAGSTMessage.h"
#include "NMEAMsgType.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEATime.h"
#include "NMEATenthsUInt16.h"
#include "NMEAMessageBuffer.h"

#include "Util/StringTools.h"
#include "Util/PlacementNew.h"
#include "Util/Logger.h"

NMEAGSTMessage::NMEAGSTMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGSTMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarning << talker << " GST message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!time.extract(nmeaLine, talker, "GST")) {
        return false;
    }

    if (!standardDeviationOfRangeInputsRMS.extract(nmeaLine, talker, "GST",
                                                   "RMS Value of Standard Deviation of Range "
                                                   "Inputs")) {
        return false;
    }

    if (!standardDeviationOfSemiMajorAxis.extract(nmeaLine, talker, "GST",
                                                  "Standard Deviation of Semi-Major Axis of Error "
                                                  "Ellipse", true)) {
        return false;
    }

    if (!standardDeviationOfSemiMinorAxis.extract(nmeaLine, talker, "GST",
                                                  "Standard Deviation of Semi-Minor Axis of Error "
                                                  "Ellipse", true)) {
        return false;
    }

    if (!orientationOfSemiMajorAxis.extract(nmeaLine, talker, "GST",
                                            "Orientation of Semi-Major Axis of Error Ellipse",
                                            true)) {
        return false;
    }

    if (!standardDeviationOfLatitudeError.extract(nmeaLine, talker, "GST",
                                                  "Standard Deviation of Latitude Error")) {
        return false;
    }

    if (!standardDeviationOfLongitudeError.extract(nmeaLine, talker, "GST",
                                                   "Standard Deviation of Longitude Error")) {
        return false;
    }

    if (!standardDeviationOfAltitudeError.extract(nmeaLine, talker, "GST",
                                                  "Standard Deviation of Altitude Error")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAGSTMessage::type() const {
    return NMEA_MSG_TYPE_GST;
}

void NMEAGSTMessage::log() const {
    logger << logDebugNMEA << talker << " GST: " << time << " SD of Range of Inputs RMS "
           << standardDeviationOfRangeInputsRMS << " SD of Semi-Major Axis "
           << standardDeviationOfSemiMajorAxis << "m SD of Semi-Minor Axis "
           << standardDeviationOfSemiMinorAxis << "m Orientation of Semi Major Axis "
           << orientationOfSemiMajorAxis << " SD of Latitude Error "
           << standardDeviationOfLatitudeError << "m SD of Longitude Error "
           << standardDeviationOfLongitudeError << "m SD of Atitude Error "
           << standardDeviationOfAltitudeError << "m" << eol;
}

NMEAGSTMessage *parseNMEAGSTMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGSTMessage *message = new (nmeaMessageBuffer)NMEAGSTMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
