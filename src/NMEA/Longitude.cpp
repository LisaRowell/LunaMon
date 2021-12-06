#include <Arduino.h>

#include "Longitude.h"


bool Longitude::set(const String &string, const String &eastOrWestStr) {
  if (string.length() < 5) {
    return false;
  }

  if (!setDegrees(string, 0, 3, 180)) {
    return false;
  }

  if (!setMinutes(string, 3)) {
    return false;
  }

  if (eastOrWestStr == "E") {
    eastOrWest = EAST;
  } else if (eastOrWestStr == "W") {
    eastOrWest = WEST;
  } else {
    return false;
  }

  return true;
}

void Longitude::print() {
  Coordinate::print();

  Serial.print(" ");

  switch (eastOrWest) {
    case EAST:
      Serial.print("E");
      break;

    case WEST:
      Serial.print("W");
  }
}