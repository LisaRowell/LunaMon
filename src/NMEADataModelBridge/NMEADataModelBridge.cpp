#include <Arduino.h>

#include "NMEADataModelBridge.h"

#include "NMEA/NMEAMessage.h"
#include "NMEA/NMEAMsgType.h"
#include "NMEA/NMEAGGAMessage.h"
#include "NMEA/NMEAGLLMessage.h"
#include "NMEA/NMEAGSAMessage.h"
#include "NMEA/NMEAGSTMessage.h"
#include "NMEA/NMEAGSVMessage.h"
#include "NMEA/NMEARMCMessage.h"
#include "NMEA/NMEAVDMVDOMessage.h"
#include "NMEA/NMEAVTGMessage.h"

#include "DataModel/DataModel.h"

#include "StatsManager/StatCounter.h"
#include "StatsManager/StatsManager.h"

#include "Util/PassiveTimer.h"
#include "Util/Logger.h"

NMEADataModelBridge::NMEADataModelBridge(StatsManager &statsManager) : messagesBridgedCounter() {
    statsManager.addStatsHolder(this);
}

void NMEADataModelBridge::processMessage(NMEAMessage *message) {
    // Add a filter here so that messages with redundant content are having their content sent
    // unnecessarily.
    const NMEAMsgType msgType = message->type();
    switch (msgType) {
        case NMEA_MSG_TYPE_GGA:
            bridgeNMEAGGAMessage((NMEAGGAMessage *)message);
            break;

        case NMEA_MSG_TYPE_GLL:
            bridgeNMEAGLLMessage((NMEAGLLMessage *)message);
            break;

        case NMEA_MSG_TYPE_GSA:
            bridgeNMEAGSAMessage((NMEAGSAMessage *)message);
            break;

        case NMEA_MSG_TYPE_GST:
            bridgeNMEAGSTMessage((NMEAGSTMessage *)message);
            break;

        case NMEA_MSG_TYPE_RMC:
            bridgeNMEARMCMessage((NMEARMCMessage *)message);
            break;

        case NMEA_MSG_TYPE_VTG:
            bridgeNMEAVTGMessage((NMEAVTGMessage *)message);
            break;

        case NMEA_MSG_TYPE_GSV:
        case NMEA_MSG_TYPE_TXT:
        case NMEA_MSG_TYPE_VDM:
        case NMEA_MSG_TYPE_VDO:
            // Currently not output to clients.
            logger << logDebugNMEADataModelBridge << "Ignoring " << message->source() << " "
                   << nmeaMsgTypeName(msgType) << " message in NMEA->Data Model Bridge" << eol;
            break;

        default:
            logger << logWarning << "Unhandled " << message->source() << " "
                   << nmeaMsgTypeName(msgType) << " message in NMEA->Data Model Bridge" << eol;
    }
}

void NMEADataModelBridge::bridgeNMEAGGAMessage(NMEAGGAMessage *message) {
    message->time.publish(positionTime);
    message->latitude.publish(positionLatitude);
    message->longitude.publish(positionLongitude);
    message->gpsQuality.publish(positionGPSQuality);
    message->numberSatellites.publish(positionNumberSatellites);
    message->horizontalDilutionOfPrecision.publish(positionHorizontalDilutionOfPrecision);
    message->antennaAltitude.publish(positionAntennaAltitude);
    message->geoidalSeparation.publish(positionGeoidalSeparation);
    message->gpsDataAge.publish(positionGPSDataAge);
    message->differentialReferenceStation.publish(positionDifferentialReferenceStation);

    messagesBridgedCounter++;
}

void NMEADataModelBridge::bridgeNMEAGLLMessage(NMEAGLLMessage *message) {
    message->latitude.publish(positionLatitude);
    message->longitude.publish(positionLongitude);
    message->time.publish(positionTime);
    message->dataValid.publish(positionDataValid);
    message->faaModeIndicator.publish(positionFAAModeindicator);

    messagesBridgedCounter++;
}

void NMEADataModelBridge::bridgeNMEAGSAMessage(NMEAGSAMessage *message) {
    if (message->automaticMode) {
        positionSatelliteSelectionMode << "Automatic";
    } else {
        positionSatelliteSelectionMode << "Manual";
    }
    message->gpsFixMode.publish(positionFixMode);

    char activeSatellites[73];
    activeSatellites[0] = 0;
    unsigned satelliteIndex;
    for (satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        if (message->satelliteIDs[satelliteIndex].hasValue()) {
            char satelliteText[7];
            if (satelliteIndex < 11) {
                snprintf(satelliteText, 7, "%u,", message->satelliteIDs[satelliteIndex].getValue());
            } else {
                snprintf(satelliteText, 7, "%u", message->satelliteIDs[satelliteIndex].getValue());
            }
            strncat(activeSatellites, satelliteText, 72);
        } else {
            if (satelliteIndex < 11) {
                strncat(activeSatellites, ",", 72);
            }
        }
    }
    positionActiveSatellites << activeSatellites;

    message->pdop.publish(positionPDOP);
    message->hdop.publish(positionHDOP);
    message->vdop.publish(positionVDOP);

    messagesBridgedCounter++;
}
void NMEADataModelBridge::bridgeNMEAGSTMessage(NMEAGSTMessage *message) {
    message->standardDeviationOfRangeInputsRMS.publish(positionStandardDeviationOfRangeInputsRMS);
    message->standardDeviationOfSemiMajorAxis.publish(positionStandardDeviationOfSemiMajorAxis);
    message->standardDeviationOfSemiMinorAxis.publish(positionStandardDeviationOfSemiMinorAxis);
    message->orientationOfSemiMajorAxis.publish(positionOrientationOfSemiMajorAxis);
    message->standardDeviationOfLatitudeError.publish(positionStandardDeviationOfLatitudeError);
    message->standardDeviationOfLongitudeError.publish(positionStandardDeviationOfLongitudeError);
    message->standardDeviationOfAltitudeError.publish(positionStandardDeviationOfAltitudeError);

    messagesBridgedCounter++;
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

    messagesBridgedCounter++;
}

void NMEADataModelBridge::bridgeNMEAVTGMessage(NMEAVTGMessage *message) {
    message->trackMadeGood.publish(positionTrackMadeGood);
    message->speedOverGround.publish(positionSpeedOverGround);
    message->faaModeIndicator.publish(positionFAAModeindicator);

    messagesBridgedCounter++;
}

void NMEADataModelBridge::exportStats(uint32_t msElapsed) {
    messagesBridgedCounter.update(nmeaDataModelMessagesBridged, nmeaDataModelMessageBridgeRate,
                                  msElapsed);
}
