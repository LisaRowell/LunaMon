#include <Arduino.h>

#include "NMEAMsgType.h"

enum NMEAMsgType parseNMEAMsgType(String &msgTypeStr) {
    if (msgTypeStr == "GLL") {
        return NMEA_MSG_TYPE_GLL;
    } else {
        return NMEA_MSG_TYPE_UNKNOWN;
    }
}