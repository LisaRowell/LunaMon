#include <Arduino.h>

#include "NMEASource.h"
#include "Util/CharacterTools.h"
#include "NMEALine.h"
#include "NMEAMessageHandler.h"
#include "Util/Logger.h"

NMEASource::NMEASource(Stream &stream, NMEAMessageHandler &messageHandler) :
              stream(stream), messageHandler(messageHandler) {
  bufferPos = 0;
  remaining = 0;
  carriageReturnFound = false;
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
  // Since NMEA 0183 has CR/LF terminated lines, if the last thing we looked
  // at was a carriage return, we should be looking at a line feed.
  if (carriageReturnFound) {
    if (isLineFeed(buffer[bufferPos])) {
      // Skip past the line feed and return saying the line is ready for processing.
      bufferPos++;
      remaining--;
      carriageReturnFound = false;
      return true;
    } else {
      // We had a carriage return without the associated line feed. Toss out any
      // characters that we had accumulated in the line and move on, processing
      // the buffer.
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
        // We had a carriage return without the associated line feed. Toss out any characters that
        // we had accumulated in the line. If we still have characters in the buffer, recursively
        // process those.
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
      // The buffer ended on a carriage return, we'll pick up looking for the
      // line feed after the next read.
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
  } else {
    messageHandler.processLine(inputLine);
  }

  inputLine.reset();
}

void NMEASource::service() {
  if (remaining) {
    if (processBuffer()) {
      lineCompleted();
      return;
    }
  }

  if (readAvailableInput()) {
    if (processBuffer()) {
      lineCompleted();
    }
  }
}