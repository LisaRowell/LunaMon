#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEAMessageHandler.h"
#include "NMEASource.h"


NMEAMessageHandler nmeaMessageHandler;
NMEASource usbSerialNMEASource(Serial, nmeaMessageHandler);

void setup() {
  Serial.begin(9600);
}

void loop() {
  usbSerialNMEASource.service();
}