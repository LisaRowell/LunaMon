#ifndef NMEA_VDM_MESSAGE_H
#define NMEA_VDM_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAUInt8.h"
#include "NMEAUInt16.h"
#include "NMEARadioChannelCode.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMsgType.h"

class NMEAVDMMessage : public NMEAMessage {
    private:
        NMEAUInt8 totalFragments;
        NMEAUInt8 fragmentNumber;
        NMEAUInt16 messageId;
        NMEARadioChannelCode radioChannelCode;

    public:
        NMEAVDMMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() override;
        virtual void log() override;

    friend class NMEADataModelBridge;
};

extern NMEAVDMMessage *parseNMEAVDMMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
