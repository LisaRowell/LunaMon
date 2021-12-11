#ifndef NMEAMSGTYPE_H
#define NMEAMSGTYPE_H

enum NMEAMsgType {
    NMEA_MSG_TYPE_UNKNOWN,
    NMEA_MSG_TYPE_GLL
};

enum NMEAMsgType parseNMEAMsgType(String &msgTypeStr);

#endif