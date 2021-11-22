#ifndef NMEATALKER_H
#define NMEATALKER_H

enum NMEATalker {
  NMEA_TALKER_UNKNOWN,
  NMEA_TALKER_GPS
};

enum NMEATalker parseNMEATalker(String &talkerCode);
void printNMEATalker(enum NMEATalker talker);

#endif