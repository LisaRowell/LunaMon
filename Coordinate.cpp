#include <Arduino.h>

#include "Coordinate.h"
#include "StringTools.h"


uint32_t Coordinate::tenthsSecondScaler() {
  uint32_t scaler = 1;
  int i;
  for (i = 0; i < (minutePrecision - 1); i++) {
    scaler = scaler * 10;
  }
  return scaler;
}

uint16_t Coordinate::tenthsOfSeconds() {
  if (minutePrecision == 0) {
    return 0;
  } else {
    uint32_t tenthsSecondScaler = Coordinate::tenthsSecondScaler();
    return (60 * minuteFraction + tenthsSecondScaler / 2) / tenthsSecondScaler;
  }
}

bool Coordinate::setDegrees(const String &string, unsigned startDegrees, unsigned endDegrees, uint8_t maxDegrees) {
  if (!extractUInt8FromString(string, startDegrees, endDegrees, degrees, maxDegrees)) {
    return false;
  }

  return true;
}

bool Coordinate::setMinutes(const String &string, unsigned startMinutes) {
  const unsigned endWholeMinutes = startMinutes + 2;
  if (!extractUInt8FromString(string, startMinutes, endWholeMinutes, minutes, 59)) {
    return false;
  }

  if (string.length() > endWholeMinutes) {
    if (string.charAt(endWholeMinutes) != '.') {
      return false;
    }

    const unsigned startMinuteFraction = endWholeMinutes + 1;
    const unsigned endMinuteFraction = string.length();
    if (!extractUInt32FromString(string, startMinuteFraction, endMinuteFraction, minuteFraction)) {
      return false;
    }
    minutePrecision = endMinuteFraction - startMinuteFraction;
  } else {
    minuteFraction = 0;
    minutePrecision = 0;
  }

  return true;
}

void Coordinate::print() {
  Serial.print(degrees);
  Serial.print("\xC2\xB0");     // Degree symbol
  Serial.print(minutes);
  Serial.print("'");
   
  uint16_t tenthsOfSeconds = Coordinate::tenthsOfSeconds();
  Serial.print(tenthsOfSeconds / 10);
  Serial.print(".");
  Serial.print(tenthsOfSeconds % 10);
}