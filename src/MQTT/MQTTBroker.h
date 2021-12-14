#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include <WiFiNINA.h>

#include "WiFiManager/WiFiManagerClient.h"
#include "WiFiManager/WiFiManager.h"
#include "MQTTConnection.h"
#include "MQTTSession.h"
#include "MQTTMessage.h"

const unsigned maxMQTTSessions = 5;

class MQTTBroker : WiFiManagerClient {
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

        void checkForLostConnections();
        void cleanupLostConnection(MQTTConnection &connection);
        void invalidateSession(MQTTSession *session);
        void serviceSessions();
        void serviceWiFiClientWithData(WiFiClient &wifiClient);
        MQTTConnection *findExistingConnection(WiFiClient &wifiClient);
        MQTTConnection *newConnection(WiFiClient &wifiClient);
        MQTTSession *findMatchingSession(const char *clientID);
        MQTTSession *findAvailableSession();
        void refuseIncomingWiFiClient(WiFiClient &wifiClient);
        void terminateConnection(MQTTConnection *connection);
        void messageReceived(MQTTConnection *connection, MQTTMessage &message);
        void connectMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void reservedMsgReceivedError(MQTTConnection *connection, MQTTMessage &message);
        void subscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void unsubscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void pingRequestMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void disconnectMessageReceived(MQTTConnection *connection, MQTTMessage &message);
        void serverOnlyMsgReceivedError(MQTTConnection *connection, MQTTMessage &message);

    public:
        MQTTBroker();
        void begin(WiFiManager &wifiManager);
        void service();
        void wifiConnected() override;
        void wifiDisconnected() override;
};

#endif