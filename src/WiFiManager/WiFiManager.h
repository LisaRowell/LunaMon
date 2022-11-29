#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WiFiManagerClient;

#include "Util/PassiveTimer.h"
#include "Util/TimeConstants.h"

#include <etl/vector.h>
#include <etl/string.h>
#include <stdint.h>

class WiFiManager {
    private:
        enum WiFiConnectionState {
            WIFI_CONNECTION_NEVER,
            WIFI_CONNECTION_WAIT,
            WIFI_CONNECTION_CONNECTED
        };

        static const size_t maxFirmwareVersionLength = 10;

        static const uint32_t connectionRetryTime = oneSecond;

        enum WiFiConnectionState connectionState;
        PassiveTimer connectionWaitTimer;

        static constexpr size_t maxClients = 5;
        etl::vector<WiFiManagerClient *, maxClients> clients;

        void verifyWiFiPresent();
        void checkFirmwareVersion();
        void initiateConnection();
        void checkConnectionStatus();
        void checkForConnectionCompleted();
        void connectionEstablished();
        void notifyWiFiConnected();
        void notifyWiFiDisconnected();
        void firmwareVersionError(const etl::istring &firmwareVersion) __attribute__((noreturn));
        void malformedFirmwareVersion(const etl::istring &firmwareVersion, const char *explanation)
                __attribute__((noreturn));

    public:
        WiFiManager();
        void begin();
        void service();
        bool connected();
        void registerForNotifications(WiFiManagerClient *client);
};

#endif
