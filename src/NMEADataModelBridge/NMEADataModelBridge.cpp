#include <Arduino.h>

#include "NMEADataModelBridge.h"

#include "NMEA/NMEAMessage.h"
#include "NMEA/NMEAMsgType.h"
#include "NMEA/NMEAGLLMessage.h"
#include "NMEA/NMEARMCMessage.h"

#include "DataModel/DataModel.h"

void NMEADataModelBridge::processMessage(NMEAMessage *message) {
    // Add a filter here so that messages with redundant content are having their content sent
    // unnecessarily.
    switch (message->type()) {
        case NMEA_MSG_TYPE_GLL:
            bridgeNMEAGLLMessage((NMEAGLLMessage *)message);
            break;

        case NMEA_MSG_TYPE_RMC:
            bridgeNMEARMCMessage((NMEARMCMessage *)message);
            break;

        case NMEA_MSG_TYPE_TXT:
            break;

        default:
            break;
    }
}

void NMEADataModelBridge::bridgeNMEAGLLMessage(NMEAGLLMessage *message) {
    message->latitude.publish(positionLatitude);
    message->longitude.publish(positionLongitude);
    message->time.publish(positionTime);
    message->dataValid.publish(positionDataValid);
    message->faaModeIndicator.publish(positionFAAModeindicator);
}

void NMEADataModelBridge::bridgeNMEARMCMessage(NMEARMCMessage *message) {
    message->time.publish(positionTime);
    message->dataValid.publish(positionDataValid);
    message->latitude.publish(positionLatitude);
    message->longitude.publish(positionLongitude);
    message->speedOverGround.publish(positionSpeedOverGround);
    message->trackMadeGood.publish(positionTrackMadeGood);
    message->date.publish(positionDate);
    message->magneticVariation.publish(positionMagneticVariation);
    message->faaModeIndicator.publish(positionFAAModeindicator);
}
