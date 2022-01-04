#include <Arduino.h>

#include "NMEA/NMEASource.h"

#include "WiFiManager/WiFiManager.h"

#include "NMEAWiFiSource/NMEAWiFiSource.h"

#include "MQTT/MQTTBroker.h"

#include "DataModel/DataModel.h"

#include "NMEADataModelBridge/NMEADataModelBridge.h"

#include "Util/TimeConstants.h"

NMEASource usbSerialNMEASource(Serial);
WiFiManager wifiManager;
NMEAWiFiSource nmeaWiFiSource(wifiManager);
MQTTBroker mqttBroker;
DataModel dataModel;
NMEADataModelBridge nmeaDataModelBridge;

void setup() {
    controllerUpTime = millis() / msInSecond;

    usbSerialNMEASource.addMessageHandler(nmeaDataModelBridge);
    nmeaWiFiSource.addMessageHandler(nmeaDataModelBridge);

    Serial.begin(9600);

    // For the time being, wait to get started until serial connects so that initial debug messages
    // don't get lost. Later we won't want this...
    while (!Serial);

    wifiManager.begin();
    mqttBroker.begin(wifiManager);
    nmeaWiFiSource.begin();
}

void loop() {
    wifiManager.service();
    usbSerialNMEASource.service();
    nmeaWiFiSource.service();
    mqttBroker.service();

    uint32_t currentUpTime = millis() / msInSecond;
    if ((currentUpTime % 10 == 0) && (currentUpTime != controllerUpTime)) {
        controllerUpTime = currentUpTime;
    }
}
