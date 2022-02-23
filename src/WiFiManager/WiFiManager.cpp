#include "WiFiManager.h"
#include "WiFiManagerClient.h"

#include "Config.h"

#include "Util/PassiveTimer.h"
#include "Util/Error.h"
#include "Util/Logger.h"

#include <etl/vector.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <WiFiNINA.h>
#include <stdint.h>

using etl::vector;
using etl::string;
using etl::istring;
using etl::string_stream;

WiFiManager::WiFiManager() : connectionState(WIFI_CONNECTION_NEVER), clients() {
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
    if (clients.full()) {
        string<80> error;
        string_stream errorStream(error);
        errorStream << "Attempt to add more than " << maxClients << " WiFi clients.";
        fatalError(error);
    }

    clients.push_back(client);

    // If, by chance, we're already connected to the network, make sure that the new client is
    // informed.
    if (connected()) {
        client->wifiConnected();
    }
}

void WiFiManager::notifyWiFiConnected() {
    for (WiFiManagerClient *client : clients) {
        client->wifiConnected();
    }
}

void WiFiManager::notifyWiFiDisconnected() {
    for (WiFiManagerClient *client : clients) {
        client->wifiDisconnected();
    }
}

void WiFiManager::verifyWiFiPresent() {
    if (WiFi.status() == WL_NO_SHIELD) {
        fatalError("WiFi shield not present");
    }
}

void WiFiManager::checkFirmwareVersion() {
    const string<maxFirmwareVersionLength> firmwareVersion(WiFi.firmwareVersion());
    const size_t firmwareVersionLength = firmwareVersion.length();

    const size_t firstPeriodPos = firmwareVersion.find('.');
    if (firstPeriodPos == firmwareVersion.npos) {
        malformedFirmwareVersion(firmwareVersion, "No first period");
    }

    const string<maxFirmwareVersionLength> majorVersion(firmwareVersion, 0, firstPeriodPos);

    const size_t startMinorVersion = firstPeriodPos + 1;
    if (startMinorVersion >= firmwareVersionLength) {
        malformedFirmwareVersion(firmwareVersion, "Just major version");
    }
    const size_t secondPeriodPos = firmwareVersion.find('.', startMinorVersion);
    if (secondPeriodPos == firmwareVersion.npos) {
        malformedFirmwareVersion(firmwareVersion, "No second period.");
    }
    const size_t minorVersionLength = secondPeriodPos - startMinorVersion;

    const string<maxFirmwareVersionLength> minorVersion(firmwareVersion, startMinorVersion,
                                                        minorVersionLength);

    const size_t startReleaseVersion = secondPeriodPos + 1;
    if (startReleaseVersion >= firmwareVersionLength) {
        malformedFirmwareVersion(firmwareVersion, "No release version");
    }

    const string<maxFirmwareVersionLength> releaseVersion(firmwareVersion, startReleaseVersion);

    if (majorVersion < "1") {
        firmwareVersionError(firmwareVersion);
    } else if (majorVersion == "1") {
        if (minorVersion < "4") {
            firmwareVersionError(firmwareVersion);
        } else if (minorVersion == "4") {
            if (releaseVersion < "8") {
                firmwareVersionError(firmwareVersion);
            }
        }
    }

    logger << logDebugWiFiManager << "Firmware version: " << firmwareVersion << eol;
}

void WiFiManager::firmwareVersionError(const istring &firmwareVersion) {
    string<80> error;
    string_stream errorStream(error);
    errorStream << "Insufficient firmware version, " << firmwareVersion << ", upgrade.";
    fatalError(error);
}

void WiFiManager::malformedFirmwareVersion(const istring &firmwareVersion,
                                           const char *explanation) {
    string<80> error;
    string_stream errorStream(error);
    errorStream << "Malformed firmware version: " << firmwareVersion << " " << explanation;
    fatalError(error);
}
