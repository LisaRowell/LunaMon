#include "NMEARadioChannelCode.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"

#include <etl/string_view.h>

bool NMEARadioChannelCode::set(const etl::string_view &radioChannelCodeView) {
    if (radioChannelCodeView.size() != 1) {
        return false;
    }
    switch (radioChannelCodeView.front()) {
        case 'A':
        case '1':
            radioChannelCode = RADIO_CHANNEL_87B;
            return true;
        case 'B':
        case '2':
            radioChannelCode = RADIO_CHANNEL_88B;
            return true;
        default:
            return false;
    }
}

bool NMEARadioChannelCode::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view radioChannelCodeView;
    if (!nmeaLine.getWord(radioChannelCodeView)) {
        logger << logWarning << talker << " " << msgType
               << " message missing Radio Channel Code field" << eol;
        return false;
    }
    if (!set(radioChannelCodeView)) {
        logger << logWarning << talker << " " << msgType
               << " message with bad Radio Channel Code field '" << radioChannelCodeView << "'"
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
