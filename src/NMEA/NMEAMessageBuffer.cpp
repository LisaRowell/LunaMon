#include "NMEAMessageBuffer.h"

#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAGSVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVDMVDOMessage.h"
#include "NMEAVTGMessage.h"

#include <stdint.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

#define NMEA_MESSAGE_BUFFER_SIZE \
(MAX(MAX(MAX(MAX(sizeof(NMEAGGAMessage), sizeof(NMEAGSAMessage)), \
             MAX(sizeof(NMEAGLLMessage), sizeof(NMEAGSTMessage))), \
         MAX(MAX(sizeof(NMEAGSVMessage), sizeof(NMEARMCMessage)), \
             MAX(sizeof(NMEATXTMessage), sizeof(NMEAVDMVDOMessage)))), \
     sizeof(NMEAVTGMessage)))

uint8_t nmeaMessageBuffer[NMEA_MESSAGE_BUFFER_SIZE];
