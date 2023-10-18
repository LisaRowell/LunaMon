/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
NMEASource usbSerialNMEASource(Serial, sysNMEAUSBMessages, sysNMEAUSBMessageRate, statsManager);
WiFiManager wifiManager;
NMEAWiFiSource nmeaWiFiSource(wifiManager, sysNMEAWiFiMessages, sysNMEAWiFiMessageRate,
                              sysNEMAWiFiState, statsManager);
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
