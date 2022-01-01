#include <Arduino.h>

#include "NMEAMsgType.h"

enum NMEAMsgType parseNMEAMsgType(String &msgTypeStr) {
    if (msgTypeStr == "GLL") {
        return NMEA_MSG_TYPE_GLL;
    } else if (msgTypeStr == "RMC") {
        return NMEA_MSG_TYPE_RMC;
    } else if (msgTypeStr == "TXT") {
        return NMEA_MSG_TYPE_TXT;
    } else {
        return NMEA_MSG_TYPE_UNKNOWN;
    }
}
