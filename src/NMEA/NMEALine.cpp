#include "NMEALine.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"

#include <stddef.h>
#include <Arduino.h>

NMEALine::NMEALine() : line() {
    position = 0;
    remaining = 0;
}

NMEALine::NMEALine(String &inputString) : line() {
    inputString.trim();

    line.initialize_free_space();
    inputString.toCharArray(line.data(), maxNMEALineLength);
    line.trim_to_terminator();
    position = 0;
    remaining = line.size();
}

void NMEALine::reset() {
    line.clear();
    position = 0;
    remaining = 0;
}

void NMEALine::append(const char *srcBuffer, unsigned start, unsigned end) {
    line.append(srcBuffer + start, end - start);
    remaining = line.size();
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
    if (remaining < 1) {
        return false;
    }

    character = line[position];
    position++;
    remaining--;

    return true;
}

String NMEALine::bufferSubstring(unsigned start, unsigned end) {
    char word[maxNMEALineLength];
    unsigned wordPos;
    unsigned bufferPos;

    for (bufferPos = start, wordPos = 0; bufferPos < end; bufferPos++, wordPos++) {
        word[wordPos] = line[bufferPos];
    }
    word[wordPos] = 0;

    return String(word);
}

bool NMEALine::extractWord(String &word) {
    unsigned pos;
    unsigned left;

    for (pos = position, left = remaining; left > 0; pos++, left--) {
        if (line[pos] == ',') {
            word = bufferSubstring(position, pos);
            position = pos + 1;
            remaining = left - 1;

            return true;
        }
    }

    if (pos != position) {
        word = bufferSubstring(position, pos);
        position = pos;
        remaining = 0;

        return true;
    } else {
        return false;
    }
}

bool NMEALine::extractWord(etl::istring &word) {
    size_t pos;
    size_t left;
    for (pos = position, left = remaining; left > 0; left--) {
        if (line[pos] == ',') {
            word.assign(line, position, pos - position);
            position = pos + 1;
            remaining = left - 1;

            return true;
        }
    }

    if (pos != position) {
        word.assign(line, position, pos - position + 1);
        position = pos;
        remaining = 0;

        return true;
    } else {
        return false;
    }
}

void NMEALine::stripParity() {
    remaining -= 3;
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