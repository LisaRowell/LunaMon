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

        static const uint32_t connectionRetryTime = oneSecond * 2;

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
