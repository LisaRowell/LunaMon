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

#include "NMEAWiFiSource.h"

#include "Config.h"

#include "WiFiManager/WiFiManager.h"

#include "DataModel/DataModelLeaf.h"
#include "DataModel/DataModelBoolLeaf.h"

#include "StatsManager/StatsManager.h"

#include "Util/Logger.h"
#include "Util/Error.h"

NMEAWiFiSource::NMEAWiFiSource(WiFiManager &wifiManager, DataModelLeaf &messageCountDataModelLeaf,
                               DataModelLeaf &messageRateDataModelLeaf,
                               DataModelBoolLeaf &connectionStatusLeaf, StatsManager &statsManager)
    : NMEASource(client, messageCountDataModelLeaf, messageRateDataModelLeaf, statsManager),
      wifiManager(wifiManager), clientConnected(false), connectionStatusLeaf(connectionStatusLeaf) {
    connectionStatusLeaf = false;
}

void NMEAWiFiSource::begin() {
    wifiManager.registerForNotifications(this);
}

void NMEAWiFiSource::service() {
    if (!clientConnected) {
        if (wifiManager.connected()) {
            if (client.connected()) {
                logger << logNotify << "Connected NMEA WiFi Client " << nmeaWiFiSourceIPAddress
                       << ":" << nmeaWiFiSourceTCPPort << eol;
                clientConnected = true;
                connectionStatusLeaf = true;
            }
        }
    }

    if (clientConnected) {
        // Make sure the other side hasn't hung up on us.
        if (!client.connected()) {
            clientConnected = false;
            connectionStatusLeaf = false;
            logger << logNotify << "NMEA WiFi Client " << nmeaWiFiSourceIPAddress << ":"
                   << nmeaWiFiSourceTCPPort << " disconnected. Retrying..." << eol;
            connect();
        }

        if (clientConnected) {
            NMEASource::service();
        }
    }
}

void NMEAWiFiSource::wifiConnected() {
    connect();
}

void NMEAWiFiSource::connect() {
    IPAddress ipAddress;
    if (!ipAddress.fromString(nmeaWiFiSourceIPAddress)) {
        fatalError("Bad NMEA WiFi Source IP Address: ");
    }

    if (client.connect(ipAddress, nmeaWiFiSourceTCPPort)) {
        clientConnected = true;
        connectionStatusLeaf = true;

        logger << logNotify << "Connected NMEA WiFi Client " << nmeaWiFiSourceIPAddress << ":"
               << nmeaWiFiSourceTCPPort << eol;
    } else {
        // It's believed that the WiFiNINA library will initiate a connection, wait for a bit to
        // see if it connects (comments say 4 sec, code does 10), and return either a success or
        // on timeout, a failure, but will still leave the TCP/IP offload still trying to connect.
        // If this is true, it's counter productive to retry the connect, we should just check to
        // see if the client is connected whenever we want to service it.
        // In the future, it would be good to extend the WiFiNINA library to not wait at all for a
        // connection...
    }
}

void NMEAWiFiSource::wifiDisconnected() {
    logger << logNotify << "Lost WiFi, disconnecting NMEA WiFi Client" << eol;
    client.stop();
}

