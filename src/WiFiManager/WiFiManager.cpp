#include <Arduino.h>
#include <WiFiNINA.h>

#include "WiFiManager.h"
#include "WiFiManagerClient.h"
#include "Config.h"
#include "Util/PassiveTimer.h"
#include "Util/Error.h"
#include "Util/Logger.h"

WiFiManager::WiFiManager() : connectionState(WIFI_CONNECTION_NEVER), numberClients(0) {
}

void WiFiManager::begin() {
    verifyWiFiPresent();
    checkFirmwareVersion();

    // The WiFiNINA library will sit and spin waiting for a begin method WiFi connection attempt to
    // succeed. While this is fine for toy sketches, and for uses where a connection is established
    // before anything else happens, it's not appropriate for when we lose our connection while
    // devices are streaming data at us. Set a "timeout" that's small so that the method will return
    // quickly. If it didn't connect in that short time (likely), we'll periodically check the
    // status looking for a successful connection while continueing to keep the house from falling
    // down.
    WiFi.setTimeout(10);
}

void WiFiManager::service() {
    switch (connectionState) {
        case WIFI_CONNECTION_NEVER:
            initiateConnection();
            break;

        case WIFI_CONNECTION_WAIT:
            checkForConnectionCompleted();
            if (connectionWaitTimer.expired()) {
                initiateConnection();
            }
            break;

        case WIFI_CONNECTION_CONNECTED:
            checkConnectionStatus();
            break;
    }
}

void WiFiManager::initiateConnection() {
    logger << logDebugWiFiManager << "Attempting to connect to WiFi network " << wifiSSID << eol;

    if (WiFi.begin(wifiSSID, wifiPassword) == WL_CONNECTED) {
        connectionEstablished();
    } else {
        // The WiFi connection has not yet succeeded, but this isn't that surprising since we gave
        // it a short timeout to avoid locking out other processing, like collecting sensor data.
        // Set a retry timer and check back on the status when we next get called.
        connectionState = WIFI_CONNECTION_WAIT;
        connectionWaitTimer.setMilliSeconds(connectionRetryTime);
    }
}

void WiFiManager::checkForConnectionCompleted() {
    if (WiFi.status() == WL_CONNECTED) {
        connectionEstablished();
    } else {
        if (connectionWaitTimer.expired()) {
            initiateConnection();
        }
    }
}

void WiFiManager::connectionEstablished() {
    // Pigs in space.....
    logger << logNotify << "Connected to WiFi network " << wifiSSID << ", IP Address "
           << WiFi.localIP() << eol;

    connectionState = WIFI_CONNECTION_CONNECTED;
    notifyWiFiConnected();
}

void WiFiManager::checkConnectionStatus() {
    if (!connected()) {
        notifyWiFiDisconnected();
        initiateConnection();
    }
}

bool WiFiManager::connected() {
    return connectionState == WIFI_CONNECTION_CONNECTED;
}

void WiFiManager::registerForNotifications(WiFiManagerClient *client) {
    if (numberClients == maxClients) {
        fatalError("Attempt to add more than " + String(maxClients) + " WiFi clients.");
    }

    clients[numberClients] = client;
    numberClients++;

    // If, by chance, we're already connected to the network, make sure that the new client is
    // informed.
    if (connected()) {
        client->wifiConnected();
    }
}

void WiFiManager::notifyWiFiConnected() {
    unsigned clientIndex;
    for (clientIndex = 0; clientIndex < numberClients; clientIndex++) {
        WiFiManagerClient *client = clients[clientIndex];
        client->wifiConnected();
    }
}

void WiFiManager::notifyWiFiDisconnected() {
    unsigned clientIndex;
    for (clientIndex = 0; clientIndex < numberClients; clientIndex++) {
        WiFiManagerClient *client = clients[clientIndex];
        client->wifiDisconnected();
    }
}

void WiFiManager::verifyWiFiPresent() {
    if (WiFi.status() == WL_NO_SHIELD) {
        fatalError("WiFi shield not present");
    }
}

void WiFiManager::checkFirmwareVersion() {
    const String firmwareVersion = WiFi.firmwareVersion();
    const int firmwareVersionLength = firmwareVersion.length();

    const int firstPeriodPos = firmwareVersion.indexOf('.');
    if (firstPeriodPos < 0) {
        malformedFirmwareVersion(firmwareVersion, "No first period");
    }

    const String majorVersion = firmwareVersion.substring(0, firstPeriodPos);

    const int startMinorVersion = firstPeriodPos + 1;
    if (startMinorVersion >= firmwareVersionLength) {
        malformedFirmwareVersion(firmwareVersion, "Just major version");
    }
    const int secondPeriodPos = firmwareVersion.indexOf('.', startMinorVersion);
    if (secondPeriodPos < 0) {
        malformedFirmwareVersion(firmwareVersion, "No second period.");
    }

    const String minorVersion = firmwareVersion.substring(startMinorVersion, secondPeriodPos);

    const int startBuildVersion = secondPeriodPos + 1;
    if (startBuildVersion >= firmwareVersionLength) {
        malformedFirmwareVersion(firmwareVersion, "No release version");
    }

    const String releaseVersion = firmwareVersion.substring(startBuildVersion, firmwareVersion.length());

    if (majorVersion < "1") {
        firmwareVersionError(firmwareVersion);
    } else if (majorVersion == "1") {
        if (minorVersion < "1") {
            firmwareVersionError(firmwareVersion);
        }
    }

    logger << logDebugWiFiManager << "Firmware version: " << firmwareVersion << eol;
}

void WiFiManager::firmwareVersionError(const String firmwareVersion) {
    fatalError("Insufficient firmware version, " + firmwareVersion + ", upgrade.");
}

void WiFiManager::malformedFirmwareVersion(const String firmwareVersion, const String explanation) {
    fatalError("Malformed firmware version: " + firmwareVersion + " " + explanation);
}
