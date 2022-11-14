#include "NMEARadioChannelCode.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"

#include <Arduino.h>

bool NMEARadioChannelCode::set(String &radioChannelCodeStr) {
    if (radioChannelCodeStr == "A" || radioChannelCodeStr == "1") {
        radioChannelCode = RADIO_CHANNEL_87B;
        return true;
    } else if (radioChannelCodeStr == "B" || radioChannelCodeStr == "2") {
        radioChannelCode = RADIO_CHANNEL_88B;
        return true;
    } else {
        return false;
    }
}

bool NMEARadioChannelCode::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String radioChannelCodeStr;
    if (!nmeaLine.extractWord(radioChannelCodeStr)) {
        logger << logWarning << talker << " " << msgType
               << " message missing Radio Channel Code field" << eol;
        return false;
    }
    if (!set(radioChannelCodeStr)) {
        logger << logWarning << talker << " " << msgType
               << " message with bad Radio Channel Code field '" << radioChannelCodeStr << "'"
               << eol;
        return false;
    }

    return true;
}

void NMEARadioChannelCode::publish(DataModelStringLeaf &leaf) const {
    switch (radioChannelCode) {
        case RADIO_CHANNEL_87B:
            leaf = "87B";
            break;

        case RADIO_CHANNEL_88B:
            leaf = "88B";
    }
}

void NMEARadioChannelCode::log(Logger &logger) const {
    switch (radioChannelCode) {
        case RADIO_CHANNEL_87B:
            logger << "87B";
            break;

        case RADIO_CHANNEL_88B:
            logger << "88B";
    }
}
