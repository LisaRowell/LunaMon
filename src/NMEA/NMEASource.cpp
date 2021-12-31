#include <Arduino.h>

#include "NMEASource.h"
#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEAMessageHandler.h"

#include "Util/CharacterTools.h"
#include "Util/Logger.h"
#include "Util/Error.h"

NMEASource::NMEASource(Stream &stream)
    : stream(stream),
      bufferPos(0),
      remaining(0),
      carriageReturnFound(false),
      numberMessageHandlers(0) {
}

void NMEASource::addMessageHandler(NMEAMessageHandler &messageHandler) {
    if (numberMessageHandlers == maxMessageHandlers) {
        fatalError("Too many message handles for NMEA source");
    }

    messageHandlers[numberMessageHandlers] = &messageHandler;
    numberMessageHandlers++;
}


bool NMEASource::scanForCarriageReturn(unsigned &carriageReturnPos) {
    unsigned scanRemaining;
    unsigned scanPos;

    for (scanPos = bufferPos, scanRemaining = remaining;
        scanRemaining;
        scanPos++, scanRemaining--) {
        if (buffer[scanPos] == '\r') {
            carriageReturnPos = scanPos;
            return true;
        }
    }

    return false;
}

bool NMEASource::processBuffer() {
    // Since NMEA 0183 has CR/LF terminated lines, if the last thing we looked at was a carriage
    // return, we should be looking at a line feed.
    if (carriageReturnFound) {
        if (isLineFeed(buffer[bufferPos])) {
            // Skip past the line feed and return saying the line is ready for processing.
            bufferPos++;
            remaining--;
            carriageReturnFound = false;
            return true;
        } else {
            // We had a carriage return without the associated line feed. Toss out any characters
            // that we had accumulated in the line and move on, processing the buffer.
            logger << logWarning << "NMEA line with CR, but no LF. Ignoring." << eol;
            inputLine.reset();
            carriageReturnFound = false;
        }
    }

    unsigned carriageReturnPos;
    if (scanForCarriageReturn(carriageReturnPos)) {
        // We have a CR, move the characters up to that point into the line.
        inputLine.append(buffer, bufferPos, carriageReturnPos);
        remaining -= carriageReturnPos - bufferPos + 1;
        bufferPos = carriageReturnPos + 1;

        // If we have at least one more character in the buffer make sure it's an LF
        if (remaining) {
            if (isLineFeed(buffer[bufferPos])) {
                // The line was correctly terminated and should be ready to go.
                remaining--;
                bufferPos++;
                return true;
            } else {
                // We had a carriage return without the associated line feed. Toss out any
                // characters that we had accumulated in the line. If we still have characters in
                // the buffer, recursively process those.
                logger << logWarning << "NMEA line with CR, but no LF. Ignoring." << eol;
                inputLine.reset();
                remaining--;
                bufferPos++;
                if (remaining) {
                    return processBuffer();
                } else {
                    bufferPos = 0;
                    return false;
                }
            }
        } else {
            // The buffer ended on a carriage return, we'll pick up looking for the line feed after
            // the next read.
            carriageReturnFound = true;
            return false;
        }
    } else {
        // No carriage return so far, stash what we have so far in the line.
        inputLine.append(buffer, bufferPos, bufferPos + remaining);
        bufferPos = 0;
        remaining = 0;
        return false;
    }
}

bool NMEASource::readAvailableInput() {
    const unsigned available = stream.available();
    if (available) {
        const unsigned readLength = min(available, maxNMEALineLength);
        remaining = stream.readBytes(buffer, readLength);
        if (readLength != remaining) {
            logger << logWarning << "Truncated NMEA serial read" << eol;
        }
        bufferPos = 0;

        return true;
    } else {
        return false;
    }
}

void NMEASource::lineCompleted() {
    if (inputLine.isEmpty()) {
        // For now we just ignore empty input lines. Count?
        return;
    }

    if (!inputLine.sanityCheck()) {
        // Errors are logged by the sanity check.
        return;
    }

    NMEAMessage *nmeaMessage = parseNMEAMessage(inputLine);
    if (nmeaMessage != NULL) {
        nmeaMessage->log();

        unsigned msgHandledIndex;
        for (msgHandledIndex = 0; msgHandledIndex < numberMessageHandlers; msgHandledIndex++) {
            NMEAMessageHandler *messageHandler = messageHandlers[msgHandledIndex];
            messageHandler->processMessage(nmeaMessage);
        }

        // While we're done with the nmeaMessage, we don't do a free here
        // since it was allocated with a static buffer and placement new.
    }
}

void NMEASource::service() {
    if (remaining) {
        if (processBuffer()) {
            lineCompleted();
            inputLine.reset();
            return;
        }
    }

    if (readAvailableInput()) {
        if (processBuffer()) {
            lineCompleted();
            inputLine.reset();
        }
    }
}
