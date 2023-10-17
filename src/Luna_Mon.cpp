#include "NMEA/NMEASource.h"

#include "WiFiManager/WiFiManager.h"

#include "NMEAWiFiSource/NMEAWiFiSource.h"

#include "MQTT/MQTTBroker.h"

#include "DataModel/DataModel.h"

#include "NMEADataModelBridge/NMEADataModelBridge.h"

#include "StatsManager/StatsManager.h"

#include "Util/TimeConstants.h"

#include <Arduino.h>

StatsManager statsManager;
NMEASource usbSerialNMEASource(Serial, usbNMEAMessages, usbNMEAMessageRate, statsManager);
WiFiManager wifiManager;
NMEAWiFiSource nmeaWiFiSource(wifiManager, wifiNMEAMessages, wifiNMEAMessageRate, statsManager);
MQTTBroker mqttBroker(statsManager);
DataModel dataModel(statsManager);
NMEADataModelBridge nmeaDataModelBridge(statsManager);

void setup() {
    logger.setLevel(LOGGER_LEVEL_DEBUG);
    logger.enableModuleDebug(LOGGER_MODULE_WIFI_MANAGER);
    logger.enableModuleDebug(LOGGER_MODULE_NMEA);

    sysBrokerUptime = millis() / msInSecond;

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
    statsManager.service();

    uint32_t currentUpTime = millis() / msInSecond;
    if ((currentUpTime % 10 == 0) && (currentUpTime != sysBrokerUptime)) {
        sysBrokerUptime = currentUpTime;
    }
}
