#include "NMEAWiFiSource.h"

#include "Config.h"

#include "WiFiManager/WiFiManager.h"

#include "DataModel/DataModelLeaf.h"

#include "StatsManager/StatsManager.h"

#include "Util/Logger.h"
#include "Util/Error.h"

NMEAWiFiSource::NMEAWiFiSource(WiFiManager &wifiManager, DataModelLeaf &messageCountDataModelLeaf,
                               DataModelLeaf &messageRateDataModelLeaf, StatsManager &statsManager)
    : NMEASource(client, messageCountDataModelLeaf, messageRateDataModelLeaf, statsManager),
      wifiManager(wifiManager), clientConnected(false) {
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
            }
        }
    }

    if (clientConnected) {
        // Make sure the other side hasn't hung up on us.
        if (!client.connected()) {
            clientConnected = false;
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

