#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFiNINA.h>

#include "MQTTConnection.h"
#include "MQTTString.h"

//
// MQTTSession
//
// This class maintains information on a MQTT Client Session per the definition of the protocol.
// It's worth highlighting that in MQTT, a session's lifespan is not necessarily one-to-one with the
// lifespan of the TCP/IP connection that created it. This allows for clients with connection issues
// to reconnect without starting from scratch with the topics that they publish or subscribe to.
//

const unsigned maxMQTTClientIDLength = 23;

class MQTTConnection;

class MQTTSession {
    private:
        bool cleanSession;
        char clientID[maxMQTTClientIDLength + 1];
        MQTTConnection *connection;

        void unsubscribeAll();

    public:
        bool isConnected() const;
        bool matches(const char *clientID) const;
        void begin(bool cleanSession, const char *clientID, MQTTConnection *connection);
        void reconnect(bool newCleanSession, MQTTConnection *connection);
        bool disconnect();
        void service();
};

#endif