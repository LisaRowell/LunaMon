#ifndef NMEA_MSG_TYPE_H
#define NMEA_MSG_TYPE_H

enum NMEAMsgType {
    NMEA_MSG_TYPE_UNKNOWN,
    NMEA_MSG_TYPE_GGA,
    NMEA_MSG_TYPE_GLL,
    NMEA_MSG_TYPE_GSA,
    NMEA_MSG_TYPE_GST,
    NMEA_MSG_TYPE_RMC,
    NMEA_MSG_TYPE_TXT,
    NMEA_MSG_TYPE_VTG
};

enum NMEAMsgType parseNMEAMsgType(String &msgTypeStr);

#endif
