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

#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

class MQTTMessage;
class WiFiManager;

#include "MQTTConnection.h"
#include "MQTTSession.h"

#include "StatsManager/StatsManager.h"

#include "WiFiManager/WiFiManagerClient.h"

#include <etl/string.h>

#include <WiFiNINA.h>

const unsigned maxMQTTSessions = 5;

class MQTTBroker : WiFiManagerClient, public StatsHolder {
    private:
        bool wifiIsConnected;

        // While the MQTT standard supports the use of WebSockets on port 8883, supporting that is
        // outside of the scope of this project and we only accept plain TCP connections on port
        // 1883. Maybe someday...
        static const uint16_t portNumber = 1883;
        WiFiServer wifiServer;

        // We have MQTT Connections and MQTT sessions and this is probably confusing. The reason two
        // different state classes are requires is that an MQTT Client can have a lifespan that
        // exceeds the lifespan of a single TCP connection. If there is a disconnect, and the client
        // is setup accordingly, the state is maintained and the client is allowed to reconnect.
        // This new TCP connection will likely have a new port number, and potentially a new IP
        // address, making it impossible for us to associate with the old connection
        // until we receive its CONNECT message. When we get that, we can associate
        // the two together.
        MQTTConnection connections[maxMQTTSessions];
        bool connectionValid[maxMQTTSessions];
        MQTTSession sessions[maxMQTTSessions];
        bool sessionValid[maxMQTTSessions];

        // Instead of being meticulous about updating the DataModel connections and sessions debug
        // strings with changes as they happen, we just set a flag and deal with doing the update
        // in the service routine.
        bool dataModelDebugNeedsUpdating;

        uint32_t messagesReceived;
        uint32_t messagesSent;
        uint32_t publishMessagesReceived;
        uint32_t publishMessagesSent;
        uint32_t publishMessagesDropped;

        void checkForLostConnections();
        void cleanupLostConnection(MQTTConnection &connection);
        void invalidateSession(MQTTSession *session);
        void serviceSessions();
        void serviceWiFiClientWithData(WiFiClient &wifiClient);
        MQTTConnection *findExistingConnection(WiFiClient &wifiClient);
        MQTTConnection *newConnection(WiFiClient &wifiClient);
        MQTTSession *findMatchingSession(const etl::istring &clientID);
        MQTTSession *findAvailableSession();
        void refuseIncomingWiFiClient(WiFiClient &wifiClient);
        void messageReceived(MQTTConnection *connection, MQTTMessage &message);
        void connectMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void reservedMsgReceivedError(MQTTConnection *connection, MQTTMessage &message);
        void subscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void unsubscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void pingRequestMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void disconnectMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void serverOnlyMsgReceivedError(MQTTConnection *connection, MQTTMessage &message);
        bool sendMQTTConnectAckMessage(MQTTConnection *connection, bool sessionPresent,
                                       uint8_t returnCode);
        bool sendMQTTPingResponseMessage(MQTTConnection *connection);
        bool sendMQTTPublishMessage(MQTTConnection *connection, const char *topic,
                                    const char *value, bool dup, uint8_t qosLevel, bool retain,
                                    uint16_t packetId);
        bool sendMQTTUnsubscribeAckMessage(MQTTConnection *connection, uint16_t packetId);
        bool sendMQTTSubscribeAckMessage(MQTTConnection *connection, uint16_t packetId,
                                         uint8_t numberResults, uint8_t *results);
        uint8_t mqttSubscribeResult(bool success, uint8_t maxQoS);
        void updateDataModelDebugs();
        void updateConnectionDebugs();
        void updateSessionDebugs();

    public:
        MQTTBroker(StatsManager &statsManager);
        void begin(WiFiManager &wifiManager);
        void service();
        void publishToConnection(MQTTConnection *connection, etl::istring &clientID,
                                 const char *topic, const char *value, bool retainedValue);
        void terminateConnection(MQTTConnection *connection);
        void terminateSession(MQTTSession *session);
        void wifiConnected() override;
        void wifiDisconnected() override;
        virtual void exportStats(uint32_t msElapsed) override;
};

#endif
