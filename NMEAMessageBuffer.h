#ifndef NMEA_MESSAGE_BUFFER_H
#define NMEA_MESSAGE_BUFFER_H

#include "NMEAGLLMessage.h"

#define NMEA_MESSAGE_BUFFER_SIZE (sizeof(NMEAGLLMessage))

extern uint8_t nmeaMessageBuffer[NMEA_MESSAGE_BUFFER_SIZE];

#endif