#ifndef NMEA_GLL_MESSAGE_H
#define NMEA_GLL_MESSAGE_H

#include "NMEATalker.h"
#include "NMEALine.h"

extern void parseNMEAGLLMessage(NMEATalker talker, NMEALine &nmeaLine);

#endif