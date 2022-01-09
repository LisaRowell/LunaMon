#ifndef NMEA_VDMVDO_MESSAGE_H
#define NMEA_VDMVDO_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAUInt8.h"
#include "NMEAUInt16.h"
#include "NMEARadioChannelCode.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMsgType.h"

class NMEAVDMVDOMessage : public NMEAMessage {
    private:
        NMEAMsgType msgType;
        NMEAUInt8 totalFragments;
        NMEAUInt8 fragmentNumber;
        NMEAUInt16 messageId;
        NMEARadioChannelCode radioChannelCode;

        const char *msgTypeName() const;

    public:
        NMEAVDMVDOMessage(NMEATalker &talker, NMEAMsgType &msgType);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() override;
        virtual void log() override;
        bool isOwnShip() const;

    friend class NMEADataModelBridge;
};

extern NMEAVDMVDOMessage *parseNMEAVDMVDOMessage(NMEATalker &talker, NMEAMsgType &msgType,
                                                 NMEALine &nmeaLine);

#endif
