#ifndef NMEA_FAA_MODE_INDICATOR_H
#define NMEA_FAA_MODE_INDICATOR_H

enum NMEAFAAModeIndicator {
  FAA_MODE_NONE,
  FAA_MODE_AUTONOMOUS,
  FAA_MODE_DIFFERENTIAL,
  FAA_MODE_ESTIMATED,
  FAA_MODE_RTK_FLOAT,
  FAA_MODE_MANUAL,
  FAA_MODE_DATA_NOT_VALID,
  FAA_MODE_PRECISE,
  FAA_MODE_RTK_INTEGER,
  FAA_MODE_SIMULATED
};

extern bool parseNMEAFAAModeIndicator(String &faaModeStr, enum NMEAFAAModeIndicator &faaModeIndicator);
extern void printNMEAFAAModeIndicator(enum NMEAFAAModeIndicator faaModeIndicator);

#endif