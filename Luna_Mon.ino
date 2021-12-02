#include <Arduino.h>

#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEAMessageHandler.h"
#include "NMEASource.h"
#include "WiFiManager.h"
#include "MQTTBroker.h"

NMEAMessageHandler nmeaMessageHandler;
NMEASource usbSerialNMEASource(Serial, nmeaMessageHandler);
WiFiManager wifiManager;
MQTTBroker mqttBroker;

void setup() {
  Serial.begin(9600);

  // For the time being, wait to get started until serial connects to that
  // initial debug messages don't get lost. Later we won't want this...
  while (!Serial);

  wifiManager.begin();
  mqttBroker.begin(wifiManager);
}

void loop() {
  wifiManager.service();
  usbSerialNMEASource.service();
  mqttBroker.service();
}