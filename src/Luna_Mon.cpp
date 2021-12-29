#include <Arduino.h>

#include "NMEA/NMEASource.h"
#include "WiFiManager/WiFiManager.h"
#include "MQTT/MQTTBroker.h"
#include "DataModel/DataModel.h"
#include "Util/TimeConstants.h"

NMEASource usbSerialNMEASource(Serial);
WiFiManager wifiManager;
MQTTBroker mqttBroker;
DataModel dataModel;

void setup() {
    controllerUpTime = millis() / msInSecond;

    Serial.begin(9600);

    // For the time being, wait to get started until serial connects so that initial debug messages
    // don't get lost. Later we won't want this...
    while (!Serial);

    wifiManager.begin();
    mqttBroker.begin(wifiManager);
}

void loop() {
    wifiManager.service();
    usbSerialNMEASource.service();
    mqttBroker.service();

    uint32_t currentUpTime = millis() / msInSecond;
    if ((currentUpTime % 10 == 0) && (currentUpTime != controllerUpTime)) {
        controllerUpTime = currentUpTime;
    }
}
