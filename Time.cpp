#include <Arduino.h>

#include "Time.h"
#include "StringTools.h"


bool Time::set(const String &timeStr) {
  if (timeStr.length() < 6) {
    return false;
  }

  if (!extractUInt8FromString(timeStr, 0, 2, hours, 23)) {
    return false;
  }

  if (!extractUInt8FromString(timeStr, 2, 4, minutes, 59)) {
    return false;
  }

  if (!extractUInt8FromString(timeStr, 4, 6, seconds, 59)) {
    return false;
  }

  if (timeStr.length() > 6) {
    if (timeStr.charAt(6) != '.') {
      return false;
    }

    const unsigned startSecondFraction = 7;
    const unsigned endSecondFraction = timeStr.length();
    if (!extractUInt32FromString(timeStr, startSecondFraction, endSecondFraction, secondFraction)) {
      return false;
    }
    secondPrecision = endSecondFraction - startSecondFraction;
  } else {
    secondFraction = 0;
    secondPrecision = 0;
  }

  return true;
}

void Time::print() {
  Serial.print(hours / 10);
  Serial.print(hours % 10);
  Serial.print(":");
  Serial.print(minutes / 10);
  Serial.print(minutes % 10);
  Serial.print(":");
  Serial.print(seconds / 10);
  Serial.print(seconds % 10);

  if (secondPrecision) {
    Serial.print(".");
    String secondDigits(secondFraction);
    unsigned leadingZeros = secondPrecision - secondDigits.length();
    while (leadingZeros--) {
      Serial.print("0");
    }
    Serial.print(secondFraction);
  }
}