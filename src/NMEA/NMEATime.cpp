#include "NMEATime.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModel/DataModelStringLeaf.h"

#include "Util/Logger.h"
#include "Util/StringTools.h"

#include <etl/string.h>
#include <etl/string_stream.h>
#include <etl/string_view.h>

bool NMEATime::set(const etl::string_view &timeView) {
    if (timeView.size() < 6) {
        return false;
    }

    etl::string_view hoursView(timeView.begin(), 2);
    if (!extractUInt8FromStringView(timeView, 0, 2, hours, 23)) {
        return false;
    }

    if (!extractUInt8FromStringView(timeView, 2, 2, minutes, 59)) {
        return false;
    }

    if (!extractUInt8FromStringView(timeView, 4, 2, seconds, 59)) {
        return false;
    }

    if (timeView.size() > 6) {
        if (timeView[6] != '.') {
            return false;
        }

        secondPrecision = timeView.size() - 7;
        if (!extractUInt32FromStringView(timeView, 7, secondPrecision, secondFraction)) {
            return false;
        }
    } else {
        secondFraction = 0;
        secondPrecision = 0;
    }

    return true;
}

bool NMEATime::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view timeView;
    if (!nmeaLine.getWord(timeView)) {
        logger << logWarning << talker << " " << msgType << " message missing Time field" << eol;
        return false;
    }

    if (!set(timeView)) {
        logger << logWarning << talker << " " << msgType << " message with bad Time field '"
               << timeView << "'" << eol;
        return false;
    }

    return true;
}

void NMEATime::publish(DataModelStringLeaf &leaf) const {
    char secondFractionStr[12];
    buildSecondsFactionString(secondFractionStr);

    etl::string<22> timeStr;
    etl::string_stream timeStrStream(timeStr);
    timeStrStream << etl::setfill('0') << etl::setw(2) << hours << etl::setw(1) << ":"
                  << etl::setw(2) << minutes << etl::setw(1) << ":" << etl::setw(2) << seconds
                  << etl::setw(0) << secondFractionStr;

    leaf = timeStr;
}

void NMEATime::buildSecondsFactionString(char *string) const {
    if (secondPrecision > 0) {
        *string = '.';
        string++;

        char fractionDigits[11];
        sprintf(fractionDigits, "%lu", secondFraction);

        unsigned leadingZeros = secondPrecision - strlen(fractionDigits);
        while (leadingZeros--) {
            *string = '0';
            string++;
        }

        strcpy(string, fractionDigits);
    } else {
        *string = 0;
    }
}

void NMEATime::log(Logger &logger) const {
    char secondFractionStr[12];
    buildSecondsFactionString(secondFractionStr);

    etl::string<22> timeStr;
    etl::string_stream timeStrStream(timeStr);
    timeStrStream << etl::setfill('0') << etl::setw(2) << hours << etl::setw(1) << ":"
                  << etl::setw(2) << minutes << etl::setw(1) << ":" << etl::setw(2) << seconds
                  << etl::setw(0) << secondFractionStr;

    logger << timeStr;
}
