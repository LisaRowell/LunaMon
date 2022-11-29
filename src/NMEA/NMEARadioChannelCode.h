#ifndef NMEA_RADIO_CHANNEL_CODE_H
#define NMEA_RADIO_CHANNEL_CODE_H

#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/LoggableItem.h"
#include "Util/Logger.h"

#include <etl/string_view.h>

class NMEARadioChannelCode : public LoggableItem {
    private:
        enum RadioChannelCode {
            RADIO_CHANNEL_87B,
            RADIO_CHANNEL_88B
        };
        RadioChannelCode radioChannelCode;

        bool set(const etl::string_view &radioChannelCodeView);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
