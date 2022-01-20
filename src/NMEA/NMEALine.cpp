#include "NMEALine.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"

#include <Arduino.h>

NMEALine::NMEALine() {
    overflowed = false;
    length = 0;
    position = 0;
    remaining = 0;
}

NMEALine::NMEALine(String &inputString) {
    overflowed = false;
    inputString.trim();

    inputString.toCharArray(buffer, maxNMEALineLength);
    length = inputString.length();
    position = 0;
    remaining = length;
}

void NMEALine::reset() {
    overflowed = false;
    buffer[0] = 0;
    length = 0;
    position = 0;
    remaining = 0;
}

void NMEALine::append(const char *srcBuffer, unsigned start, unsigned end) {
    if (overflowed) {
        return;
    }

    const unsigned addedCharacters = end - start;
    if (remaining + addedCharacters > maxNMEALineLength) {
        overflowed = true;
        return;
    }

    unsigned srcPos;
    for (srcPos = start; srcPos < end; srcPos++) {
        buffer[length] = srcBuffer[srcPos];
        length++;
    }
    buffer[length] = 0;
    remaining = length;
}

bool NMEALine::isEmpty() {
    return length == 0;
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
        logger << logWarning << "NMEA line with bad parity: " << buffer << eol;
        return false;
    }
    stripParity();

    return true;
}

bool NMEALine::extractChar(char &character) {
    if (remaining < 1) {
        return false;
    }

    character = buffer[position];
    position++;
    remaining--;

    return true;
}

String NMEALine::bufferSubstring(unsigned start, unsigned end) {
    char word[maxNMEALineLength];
    unsigned wordPos;
    unsigned bufferPos;

    for (bufferPos = start, wordPos = 0; bufferPos < end; bufferPos++, wordPos++) {
        word[wordPos] = buffer[bufferPos];
    }
    word[wordPos] = 0;

    return String(word);
}

bool NMEALine::extractWord(String &word) {
    unsigned pos;
    unsigned left;

    for (pos = position, left = remaining; left > 0; pos++, left--) {
        if (buffer[pos] == ',') {
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

void NMEALine::stripParity() {
    remaining -= 3;
}

bool NMEALine::checkParity() {
    unsigned checksumPos = length - 3;
    if (buffer[checksumPos] != '*') {
        return false;
    }

    uint8_t checksum = 0;
    unsigned pos;
    for (pos = 1; pos < checksumPos; pos++) {
        checksum = checksum ^ buffer[pos];
    }

    const uint8_t firstChecksumChar = buffer[checksumPos + 1];
    const uint8_t secondChecksumChar = buffer[checksumPos + 2];
    if (!isUpperCaseHexidecimalDigit(firstChecksumChar) ||
        !isUpperCaseHexidecimalDigit(secondChecksumChar)) {
        return false;
    }
    const uint8_t lineChecksum = hexidecimalValue(firstChecksumChar) * 16 +
                                 hexidecimalValue(secondChecksumChar);

    return lineChecksum == checksum;
}

void NMEALine::logLine() {
    logger << logDebugNMEA;
    unsigned pos;
    for (pos = 0; pos < length; pos++) {
        logger << buffer[pos];
    }

    logger << eol;
}