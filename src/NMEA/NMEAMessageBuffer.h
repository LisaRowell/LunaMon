#ifndef NMEA_MESSAGE_BUFFER_H
#define NMEA_MESSAGE_BUFFER_H

#include "NMEAGLLMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVTGMessage.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define NMEA_MESSAGE_BUFFER_SIZE (MAX(MAX(sizeof(NMEAGLLMessage), sizeof(NMEARMCMessage)), \
                                      MAX(sizeof(NMEATXTMessage), sizeof(NMEAVTGMessage))))

extern uint8_t nmeaMessageBuffer[NMEA_MESSAGE_BUFFER_SIZE];

#endif
