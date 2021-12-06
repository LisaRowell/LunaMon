#include <Arduino.h>

#include "NMEATalker.h"


enum NMEATalker parseNMEATalker(String &talkerCode) {
  if (talkerCode == "GP") {
    return NMEA_TALKER_GPS;
  } else {
    return NMEA_TALKER_UNKNOWN;
  }
}

void printNMEATalker(enum NMEATalker talker) {
  switch (talker) {
    case NMEA_TALKER_GPS:
      Serial.print("GPS");
      break;

    case NMEA_TALKER_UNKNOWN:
      Serial.print("Unknown");
  }
}