#ifndef NMEA_MESSAGE_BUFFER_H
#define NMEA_MESSAGE_BUFFER_H

#include "NMEAGLLMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define NMEA_MESSAGE_BUFFER_SIZE (MAX(MAX(sizeof(NMEAGLLMessage), sizeof(NMEARMCMessage)), \
                                      sizeof(NMEATXTMessage)))

extern uint8_t nmeaMessageBuffer[NMEA_MESSAGE_BUFFER_SIZE];

#endif
