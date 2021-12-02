#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include <WiFiNINA.h>

#include "WiFiManagerClient.h"
#include "WiFiManager.h"
#include "MQTTConnection.h"
#include "MQTTClient.h"
#include "MQTTMessage.h"

const unsigned maxMQTTClients = 5;

class MQTTBroker : WiFiManagerClient {
  private:
    bool wifiIsConnected;

    // While the MQTT standard supports the use of WebSockets on port 8883,
    // supporting that is outside of the scope of this project and we only
    // accept plain TCP connections on port 1883. Maybe someday...
    static const uint16_t portNumber = 1883;
    WiFiServer wifiServer;

    // We have MQTT Connections and MQTT clients and this is probably confusing.
    // The reason two different state classes are requires is that an MQTT Client
    // can have a lifespan that exceeds the lifespan of a single TCP connection.
    // If there is a disconnect, and the client is setup accordingly, the state
    // is maintained and the client is allowed to reconnect. This new TCP
    // connection will likely have a new port number, and potentially a new IP
    // address, making it impossible for us to associate with the old connection
    // until we receive its CONNECT message. When we get that, we can associate
    // the two together.
    MQTTConnection connections[maxMQTTClients];
    bool connectionValid[maxMQTTClients];
    MQTTClient clients[maxMQTTClients];
    bool clientValid[maxMQTTClients];

    void serviceClients();
    void serviceWiFiClientWithData(WiFiClient &wifiClient);
    MQTTConnection *findExistingConnection(WiFiClient &wifiClient);
    MQTTConnection *newConnection(WiFiClient &wifiClient);
    void refuseIncomingWiFiClient(WiFiClient &wifiClient);
    void terminateConnection(MQTTConnection *connection);
    void messageReceived(MQTTConnection *connection, MQTTMessage &message);
    void connectMessageReceived(MQTTMessage &message);
    void reservedMsgReceivedError(MQTTConnection *connection, MQTTMessage &message);

  public:
    MQTTBroker();
    void begin(WiFiManager &wifiManager);
    void service();
    void wifiConnected() override;
    void wifiDisconnected() override;
};

#endif