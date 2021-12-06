#include <Arduino.h>

#include "NMEAGLLMessage.h"
#include "NMEAMessageBuffer.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Time.h"
#include "NMEADataValid.h"
#include "NMEAFAAModeIndicator.h"
#include "Util/PlacementNew.h"


bool NMEAGLLMessage::parse(NMEATalker talker, NMEALine &nmeaLine) {
  String latitudeStr;
  if (!nmeaLine.extractWord(latitudeStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message missing latitude");
    return false;
  }

  String northOrSouthStr;
  if (!nmeaLine.extractWord(northOrSouthStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message missing N/S");
    return false;
  }

  if (!latitude.set(latitudeStr, northOrSouthStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message with bad latitude");
    return false;
  }

  String longitudeStr;
  if (!nmeaLine.extractWord(longitudeStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message missing longitude");
    return false;
  }

  String eastOrWestStr;
  if (!nmeaLine.extractWord(eastOrWestStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message missing E/W");
    return false;
  }

  if (!longitude.set(longitudeStr, eastOrWestStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message with bad longitude");
    return false;
  }

  String timeStr;
  if (!nmeaLine.extractWord(timeStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message missing time");
    return false;
  }
  if (!time.set(timeStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message with bad time");
    return false;
  }

  String dataValidStr;
  if (!nmeaLine.extractWord(dataValidStr)) {
    printNMEATalker(talker);
    Serial.println(" GLL message missing data valid field");
    return false;
  }
  if (!parseNMEADataValid(dataValidStr, dataValid)) {
    printNMEATalker(talker);
    Serial.println(" GLL message with bad data valid field");
    return false;
  }

  String faaModeIndicatorStr;
  if (nmeaLine.extractWord(faaModeIndicatorStr)) {
    if (!parseNMEAFAAModeIndicator(faaModeIndicatorStr, faaModeIndicator)) {
      printNMEATalker(talker);
      Serial.print(" GLL message with bad FAA mode indicator '");
      Serial.print(faaModeIndicatorStr);
      Serial.println("'");
      return false;
    }
  } else {
    faaModeIndicator = FAA_MODE_NONE;
  }

  return true;
}

void NMEAGLLMessage::print() {
  Serial.print("GPS GLL: ");

  latitude.print();

  Serial.print(" ");
  longitude.print();

  Serial.print(" ");
  time.print();

  Serial.print(" ");
  printNMEADataValid(dataValid);

  if (faaModeIndicator != FAA_MODE_NONE) {
    Serial.print(" ");
    printNMEAFAAModeIndicator(faaModeIndicator);
  }

  Serial.println();
}

NMEAGLLMessage *parseNMEAGLLMessage(NMEATalker talker, NMEALine &nmeaLine) {
  NMEAGLLMessage *message = new(nmeaMessageBuffer) NMEAGLLMessage;
  if (!message) {
    return NULL;
  }

  if (!message->parse(talker, nmeaLine)) {
    // Since we use a static buffer and placement new for messages,
    // we don't do a free here.
    return NULL;
  }

  return message;
}
