#ifndef NMEA_TALKER_H
#define NMEA_TALKER_H

enum NMEATalker {
    NMEA_TALKER_UNKNOWN,
    NMEA_TALKER_GPS
};

enum NMEATalker parseNMEATalker(String &talkerCode);
const char *nmeaTalkerName(enum NMEATalker talker);

#endif