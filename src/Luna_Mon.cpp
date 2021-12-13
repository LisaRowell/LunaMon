#include <Arduino.h>

#include "NMEA/NMEAMessageHandler.h"
#include "NMEA/NMEASource.h"
#include "WiFiManager/WiFiManager.h"
#include "MQTT/MQTTBroker.h"
#include "DataModel/DataModel.h"
#include "Util/TimeConstants.h"

NMEAMessageHandler nmeaMessageHandler;
NMEASource usbSerialNMEASource(Serial, nmeaMessageHandler);
WiFiManager wifiManager;
MQTTBroker mqttBroker;
DataModel dataModel;

void setup() {
    controllerUpTime.setValue(millis() / msInSecond);

    Serial.begin(9600);

    // For the time being, wait to get started until serial connects to that initial debug messages
    // don't get lost. Later we won't want this...
    while (!Serial);

    wifiManager.begin();
    mqttBroker.begin(wifiManager);
}

void loop() {
    wifiManager.service();
    usbSerialNMEASource.service();
    mqttBroker.service();

    const uint32_t currentUpTime = millis() / msInSecond;
    if (currentUpTime != controllerUpTime.currentValue()) {
        controllerUpTime.setValue(currentUpTime);
    }
}