#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "WiFiManagerClient.h"
#include "Util/PassiveTimer.h"
#include "Util/TimeConstants.h"

class WiFiManager {
  private:
    enum WiFiConnectionState {
      WIFI_CONNECTION_NEVER,
      WIFI_CONNECTION_WAIT,
      WIFI_CONNECTION_CONNECTED
    };

    static const uint32_t connectionRetryTime = oneSecond;

    enum WiFiConnectionState connectionState;
    PassiveTimer connectionWaitTimer;

    static const unsigned maxClients = 5;
    unsigned numberClients;
    WiFiManagerClient *clients[maxClients];

    void verifyWiFiPresent();
    void checkFirmwareVersion();
    void initiateConnection();
    void checkConnectionStatus();
    void checkForConnectionCompleted();
    void connectionEstablished();
    void notifyWiFiConnected();
    void notifyWiFiDisconnected();
    void firmwareVersionError(const String firmwareVersion) __attribute__((noreturn));
    void malformedFirmwareVersion(const String firmwareVersion, const String explanation) __attribute__((noreturn));

  public:
    WiFiManager();
    void begin();
    void service();
    bool connected();
    void registerForNotifications(WiFiManagerClient *client);
};

#endif