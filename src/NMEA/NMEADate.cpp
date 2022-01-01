#include <Arduino.h>

#include "NMEADate.h"

#include "Config.h"

#include "Util/StringTools.h"
#include "Util/Logger.h"

bool NMEADate::set(const String &dateStr) {
    const unsigned length = dateStr.length();
    if (length == 0) {
        hasValue = false;
        return true;
    }

    if (length != 6) {
        return false;
    }

    if (!extractUInt8FromString(dateStr, 0, 2, day, 31)) {
        return false;
    }

    if (!extractUInt8FromString(dateStr, 2, 4, month, 12)) {
        return false;
    }

    // NMEA 0183 has a problem with date fields having a two digit year. How this made it into a
    // specification is beyond my words. Make a year by using a century in the configuration.
    uint8_t twoDigitYear;
    if (!extractUInt8FromString(dateStr, 4, 6, twoDigitYear)) {
        return false;
    }
    year = twoDigitYear + century;

    hasValue = true;
    return true;
}

bool NMEADate::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    String dateStr;
    if (!nmeaLine.extractWord(dateStr)) {
        logger << logWarning << talker << " " << msgType << " message missing Date field" << eol;
        return false;
    }

    if (!set(dateStr)) {
        logger << logWarning << talker << " " << msgType << " message with bad Date field '"
               << dateStr << "'" << eol;
        return false;
    }

    return true;
}

void NMEADate::publish(DataModelLeaf &leaf) const {
    if (hasValue) {
        // We use the US format of mm/dd/yyyyy.
        char dateStr[14];

        snprintf(dateStr, 14, "%02u/%02u/%u", month, day, year);
        leaf << dateStr;
    } else {
        leaf << "";
    }
}

void NMEADate::log(Logger &logger) const {
    if (hasValue) {
        logger << month / 10 << month % 10 << "/" << day / 10 << day % 10 << "/" << year;
    } else {
        logger << "Unknown\xC2\xB0";
    }
}
