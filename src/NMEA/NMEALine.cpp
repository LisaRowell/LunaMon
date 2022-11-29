#include "NMEALine.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"

#include <etl/string.h>
#include <etl/string_view.h>

#include <stddef.h>
#include <Arduino.h>

NMEALine::NMEALine() : line(), remaining(){
}

NMEALine::NMEALine(String &inputString) : line() {
    inputString.trim();

    line.initialize_free_space();
    inputString.toCharArray(line.data(), maxNMEALineLength);
    line.trim_to_terminator();

    remaining = line;
}

void NMEALine::reset() {
    line.clear();
    remaining = line;
}

void NMEALine::append(const char *srcBuffer, unsigned start, unsigned end) {
    line.append(srcBuffer + start, end - start);
    remaining = line;
}

bool NMEALine::isEmpty() {
    return line.empty();
}

bool NMEALine::isEncapsulatedData() {
    return encapsulatedData;
}

bool NMEALine::sanityCheck() {
    char msgStart;

    if (!extractChar(msgStart)) {
        logger << logWarning << "Empty NMEA message" << eol;
        return false;
    }

    switch (msgStart) {
        case '$':
            encapsulatedData = false;
            break;

        case '!':
            encapsulatedData = true;
            break;

        default:
            logger << logWarning << "NMEA message missing leading '$'" << eol;
            logLine();
            return false;
    }

    if (!checkParity()) {
        logger << logWarning << "NMEA line with bad parity: " << line << eol;
        return false;
    }
    stripParity();

    return true;
}

bool NMEALine::extractChar(char &character) {
    if (remaining.empty()) {
        return false;
    }

    character = remaining.front();
    remaining.remove_prefix(1);

    return true;
}

bool NMEALine::getWord(etl::string_view &word) {
    if (remaining.empty()) {
        return false;
    }

    size_t commaPos = remaining.find(',');
    if (commaPos == remaining.npos) {
        word.assign(remaining.begin(), remaining.end());
        remaining.remove_prefix(remaining.size());
    } else {
        word.assign(remaining.begin(), remaining.begin() + commaPos);
        remaining.remove_prefix(commaPos + 1);
    }

    return true;
}

bool NMEALine::atEndOfLine() {
    return remaining.empty();
}

void NMEALine::stripParity() {
    remaining.remove_suffix(3);
}

bool NMEALine::checkParity() {
    unsigned checksumPos = line.size() - 3;
    if (line[checksumPos] != '*') {
        return false;
    }

    uint8_t checksum = 0;
    unsigned pos;
    for (pos = 1; pos < checksumPos; pos++) {
        checksum = checksum ^ line[pos];
    }

    const uint8_t firstChecksumChar = line[checksumPos + 1];
    const uint8_t secondChecksumChar = line[checksumPos + 2];
    if (!isUpperCaseHexidecimalDigit(firstChecksumChar) ||
        !isUpperCaseHexidecimalDigit(secondChecksumChar)) {
        return false;
    }
    const uint8_t lineChecksum = hexidecimalValue(firstChecksumChar) * 16 +
                                 hexidecimalValue(secondChecksumChar);

    return lineChecksum == checksum;
}

void NMEALine::logLine() {
    logger << logDebugNMEA << line << eol;
}