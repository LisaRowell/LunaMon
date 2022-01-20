#include "NMEAMsgType.h"

#include "Util/Error.h"

#include <Arduino.h>

enum NMEAMsgType parseNMEAMsgType(String &msgTypeStr) {
    if (msgTypeStr == "GGA") {
        return NMEA_MSG_TYPE_GGA;
    } else if (msgTypeStr == "GLL") {
        return NMEA_MSG_TYPE_GLL;
    } else if (msgTypeStr == "GSA") {
        return NMEA_MSG_TYPE_GSA;
    } else if (msgTypeStr == "GST") {
        return NMEA_MSG_TYPE_GST;
    } else if (msgTypeStr == "GSV") {
        return NMEA_MSG_TYPE_GSV;
    } else if (msgTypeStr == "RMC") {
        return NMEA_MSG_TYPE_RMC;
    } else if (msgTypeStr == "TXT") {
        return NMEA_MSG_TYPE_TXT;
    } else if (msgTypeStr == "VDM") {
        return NMEA_MSG_TYPE_VDM;
    } else if (msgTypeStr == "VDO") {
        return NMEA_MSG_TYPE_VDO;
    } else if (msgTypeStr == "VTG") {
        return NMEA_MSG_TYPE_VTG;
    } else {
        return NMEA_MSG_TYPE_UNKNOWN;
    }
}

const char *nmeaMsgTypeName(NMEAMsgType msgType) {
    switch (msgType) {
        case NMEA_MSG_TYPE_UNKNOWN:
            return "Unknown";
        case NMEA_MSG_TYPE_GGA:
            return "GGA";
        case NMEA_MSG_TYPE_GLL:
            return "GLL";
        case NMEA_MSG_TYPE_GSA:
            return "GSA";
        case NMEA_MSG_TYPE_GST:
            return "GST";
        case NMEA_MSG_TYPE_GSV:
            return "GSV";
        case NMEA_MSG_TYPE_RMC:
            return "RMC";
        case NMEA_MSG_TYPE_TXT:
            return "TXT";
        case NMEA_MSG_TYPE_VDM:
            return "VDM";
        case NMEA_MSG_TYPE_VDO:
            return "VDO";
        case NMEA_MSG_TYPE_VTG:
            return "VTG";
        default:
            fatalError("Missing NMEA message type in name function");
    }
}
