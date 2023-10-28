/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

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
    message->time.publish(gpsTime);
    message->latitude.publish(gpsLatitude);
    message->longitude.publish(gpsLongitude);
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
    message->latitude.publish(gpsLatitude);
    message->longitude.publish(gpsLongitude);
    message->time.publish(gpsTime);
    message->dataValid.publish(gpsDataValid);
    message->faaModeIndicator.publish(gpsFAAModeindicator);

    messagesBridgedCounter++;
}

void NMEADataModelBridge::bridgeNMEAGSAMessage(NMEAGSAMessage *message) {
    if (message->automaticMode) {
        positionSatelliteSelectionMode = "Automatic";
    } else {
        positionSatelliteSelectionMode = "Manual";
    }
    message->gpsFixMode.publish(positionFixMode);

    etl::string<activeSatellitesLength> activeSatellitesStr;
    etl::string_stream activeSatellitesStrStream(activeSatellitesStr);
    bool firstInList = true;
    for (unsigned satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        if (message->satelliteIDs[satelliteIndex].hasValue()) {
            if (!firstInList) {
                activeSatellitesStrStream << ",";
            } else {
                firstInList = false;
            }
            activeSatellitesStrStream << message->satelliteIDs[satelliteIndex].getValue();
        }
    }

    positionActiveSatellites = activeSatellitesStr;

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
    message->time.publish(gpsTime);
    message->dataValid.publish(gpsDataValid);
    message->latitude.publish(gpsLatitude);
    message->longitude.publish(gpsLongitude);
    message->speedOverGround.publish(gpsSpeedOverGround);
    message->trackMadeGood.publish(gpsTrackMadeGoodTrue);
    message->date.publish(gpsDate);
    message->magneticVariation.publish(gpsMagneticVariation);
    message->faaModeIndicator.publish(gpsFAAModeindicator);

    messagesBridgedCounter++;
}

void NMEADataModelBridge::bridgeNMEAVTGMessage(NMEAVTGMessage *message) {
    message->trackMadeGoodTrue.publish(gpsTrackMadeGoodTrue);
    message->trackMadeGoodMagnetic.publish(gpsTrackMadeGoodMagnetic);
    message->speedOverGround.publish(gpsSpeedOverGround);
    message->speedOverGroundKmPerH.publish(gpsSpeedOverGroundKmPerH);
    message->faaModeIndicator.publish(gpsFAAModeindicator);

    messagesBridgedCounter++;
}

void NMEADataModelBridge::exportStats(uint32_t msElapsed) {
    messagesBridgedCounter.update(sysNMEADataModelMessagesBridged,
                                  sysNMEADataModelMessageBridgeRate, msElapsed);
}
