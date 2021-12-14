#include "MQTTBroker.h"

#include <Arduino.h>

#include "DataModel/DataModel.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnection.h"
#include "MQTTDisconnectMessage.h"
#include "MQTTMessage.h"
#include "MQTTSession.h"
#include "MQTTSubscribeAckMessage.h"
#include "MQTTSubscribeMessage.h"
#include "MQTTUnsubscribeMessage.h"
#include "MQTTUnsubscribeAckMessage.h"
#include "MQTTPingRequestMessage.h"
#include "MQTTPingResponseMessage.h"
#include "Util/Error.h"
#include "Util/Logger.h"
#include "WiFiManager/WiFiManager.h"

MQTTBroker::MQTTBroker() : wifiIsConnected(false), wifiServer(portNumber) {
    unsigned sessionPos;
    for (sessionPos = 0; sessionPos < maxMQTTSessions; sessionPos++) {
        sessionValid[sessionPos] = false;
    }
}

void MQTTBroker::begin(WiFiManager &wifiManager) { wifiManager.registerForNotifications(this); }

void MQTTBroker::service() {
    checkForLostConnections();
    serviceSessions();

    if (wifiIsConnected) {
        bool sessionWasAvailable;
        do {
            WiFiClient wifiClient = wifiServer.available();
            if (wifiClient) {
                serviceWiFiClientWithData(wifiClient);
                sessionWasAvailable = true;
            } else {
                sessionWasAvailable = false;
            }
        } while (sessionWasAvailable);
    }
}

void MQTTBroker::serviceWiFiClientWithData(WiFiClient &wifiClient) {
    // Some client has data for us. Given the wonkiness of the WiFiNINA api, we don't know if this
    // is a new to us TCP connection or one we already know about. We do know that it has data ready
    // to be read.
    MQTTConnection *connection;
    connection = findExistingConnection(wifiClient);
    if (!connection) {
        connection = newConnection(wifiClient);
        if (connection == NULL) {
            refuseIncomingWiFiClient(wifiClient);
            return;
        }
        logger << logDebug << "Established MQTT Connection from " << connection->ipAddress() << ":"
               << connection->port() << eol;
    }

    // Since we're receiving messages over TCP, even if it's a small message we have no guarantee
    // that the entire message will be available at this time. We use a buffer in the connection to
    // read in data, chunk by chunk, until an entire message is present, then process it. An
    // alternative implementation would be to use threads and let the thread block trying to read
    // message data, but it's not clear that the WiFiNINA library is thread safe (it's highly
    // unlikely) and we're trying to avoid multi-threading.
    bool errorTerminateConnection;
    MQTTMessage message;
    bool messageComplete = connection->readMessageData(message, errorTerminateConnection);
    if (errorTerminateConnection) {
        terminateConnection(connection);
        return;
    }
    if (messageComplete) {
        messageReceived(connection, message);
    }
}

void MQTTBroker::terminateConnection(MQTTConnection *connection) {
    connection->stop();

    if (connection->hasSession()) {
        MQTTSession *session = connection->session();
        bool retain = session->disconnect();
        if (!retain) {
            unsigned sessionPos;
            bool sessionFound;
            for (sessionPos = 0, sessionFound = false;
                 sessionPos < maxMQTTSessions && !sessionFound; sessionPos++) {
                if (sessionValid[sessionPos] && &sessions[sessionPos] == session) {
                    sessionFound = true;
                    sessionValid[sessionPos] = false;
                }
            }

            if (!sessionFound) {
                fatalError("Lost track of a MQTT Session and couldn't delete it");
            }
        }
    }

    unsigned connectionPos;
    for (connectionPos = 0; connectionPos < maxMQTTSessions; connectionPos++) {
        if (&connections[connectionPos] == connection) {
            connectionValid[connectionPos] = false;
            return;
        }
    }

    fatalError("Lost track of an MQTT connection");
}

void MQTTBroker::terminateSession(MQTTSession *session) {
    if (session->isConnected()) {
        logger << logError
               << "Attempted to terminate a Session that had an active Connection. Programmer error"
               << eol;
    }

    invalidateSession(session);
}

MQTTConnection *MQTTBroker::findExistingConnection(WiFiClient &wifiClient) {
    unsigned connectionIndex;
    for (connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        if (connectionValid[connectionIndex]) {
            MQTTConnection *connection = &connections[connectionIndex];
            if (connection->matches(wifiClient)) {
                return connection;
            }
        }
    }

    return NULL;
}

MQTTConnection *MQTTBroker::newConnection(WiFiClient &wifiClient) {
    unsigned connectionIndex;
    for (connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        if (!connectionValid[connectionIndex]) {
            MQTTConnection *connection = &connections[connectionIndex];
            connection->begin(wifiClient);
            connectionValid[connectionIndex] = true;
            return connection;
        }
    }

    return NULL;
}

void MQTTBroker::wifiConnected() {
    logger << logNotify << "Connected to WiFi, starting MQTT server." << eol;
    wifiIsConnected = true;
    wifiServer.begin();
}

void MQTTBroker::wifiDisconnected() {
    wifiIsConnected = false;
    fatalError("Lost WiFi connection. Code needs to be written for this...");
}

void MQTTBroker::checkForLostConnections() {
    unsigned connectionIndex;
    for (connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        if (connectionValid[connectionIndex]) {
            MQTTConnection &connection = connections[connectionIndex];
            if (connection.wasDisconnected()) {
                cleanupLostConnection(connection);
                connectionValid[connectionIndex] = false;
            }
        }
    }
}

void MQTTBroker::cleanupLostConnection(MQTTConnection &connection) {
    logger << logDebug << "Lost TCP connection from " << connection.ipAddress() << ":"
           << connection.port() << eol;
    if (connection.hasSession()) {
        MQTTSession *session = connection.session();
        bool retainConnection = session->disconnect();
        if (!retainConnection) {
            invalidateSession(session);
        }
    }
}

void MQTTBroker::invalidateSession(MQTTSession *session) {
    unsigned sessionIndex;
    for (sessionIndex = 0; sessionIndex < maxMQTTSessions; sessionIndex++) {
        if (sessionValid[sessionIndex] && &sessions[sessionIndex] == session) {
            sessionValid[sessionIndex] = false;
            return;
        }
    }

    fatalError("Lost track of a MQTT Session and couldn't invalidate it.");
}

void MQTTBroker::serviceSessions() {
    unsigned sessionIndex;

    for (sessionIndex = 0; sessionIndex < maxMQTTSessions; sessionIndex++) {
        if (sessionValid[sessionIndex]) {
            MQTTSession &session = sessions[sessionIndex];
            session.service(this);
        }
    }
}

void MQTTBroker::refuseIncomingWiFiClient(WiFiClient &wifiClient) {
    logger << logWarning
           << "Maximum number of MQTT WiFi sessions exceeded, refusing incoming connection from "
           << wifiClient.remoteIP() << ":" << wifiClient.remotePort() << eol;

    wifiClient.flush();
    wifiClient.stop();
}

void MQTTBroker::messageReceived(MQTTConnection *connection, MQTTMessage &message) {
    MQTTMessageType msgType = message.messageType();
    switch (msgType) {
        case MQTT_MSG_CONNECT:
            connectMessageReceived(connection, message);
            break;

        case MQTT_MSG_CONNACK:
        case MQTT_MSG_SUBACK:
        case MQTT_MSG_UNSUBACK:
        case MQTT_MSG_PINGRESP:
            serverOnlyMsgReceivedError(connection, message);
            break;

        case MQTT_MSG_SUBSCRIBE:
            subscribeMessageReceived(connection, message);
            break;

        case MQTT_MSG_UNSUBSCRIBE:
            unsubscribeMessageReceived(connection, message);
            break;

        case MQTT_MSG_PINGREQ:
            pingRequestMessageReceived(connection, message);
            break;

        case MQTT_MSG_DISCONNECT:
            disconnectMessageReceived(connection, message);
            break;

        case MQTT_MSG_RESERVED1:
        case MQTT_MSG_RESERVED2:
            reservedMsgReceivedError(connection, message);
            break;

        case MQTT_MSG_PUBACK:
        case MQTT_MSG_PUBREC:
        case MQTT_MSG_PUBREL:
        case MQTT_MSG_PUBCOMP:
        default:
            logger << logWarning << "Received unimplemented message type "
                   << message.messageTypeStr() << eol;
    }

    connection->resetMessageBuffer();
}

void MQTTBroker::connectMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
    // Per the MQTT specification, we treat a second CONNECT for a connection as a protocol error.
    if (connection->hasSession()) {
        logger << logWarning << "Second MQTT CONNECT received for a connection." << eol;
        terminateConnection(connection);
    }

    MQTTConnectMessage connectMessage(message);
    if (!connectMessage.parse()) {
        logger << logWarning << "Bad connect message. Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }

    uint8_t errorCode;
    errorCode = connectMessage.sanityCheck();
    if (errorCode != MQTT_CONNACK_ACCEPTED) {
        logger << logWarning << "Terminating connection due to failed CONNECT message sanity check"
               << eol;
        sendMQTTConnectAckMessage(connection, false, errorCode);
        return;
    }

    // Since this is a light weight broker, and a work in progress, we reject a few currently
    // unsupported types of connections.
    if (connectMessage.hasWill()) {
        logger << logWarning << "MQTT CONNECT with Will: Currently unsupported" << eol;
        sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
        return;
    }
    if (connectMessage.hasUserName()) {
        logger << logWarning << "MQTT CONNECT message with Password set" << eol;
        sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
        return;
    }
    if (connectMessage.hasPassword()) {
        logger << logWarning << "MQTT CONNECT message with Password set" << eol;
        sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
        return;
    }

    const MQTTString *clientIDStr = connectMessage.clientID();
    char clientID[maxMQTTClientIDLength + 1];
    if (!clientIDStr->copyTo(clientID, maxMQTTClientIDLength)) {
        logger << logWarning << "MQTT CONNECT message with too long of a Client ID:" << *clientIDStr
               << eol;
        sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED);
        return;
    }

    uint16_t keepAliveTime = connectMessage.keepAliveSec();

    MQTTSession *session = findMatchingSession(clientID);
    if (session) {
        if (session->isConnected()) {
            logger << logWarning << "MQTT CONNECT message received for a Client ID (" << clientID
                   << ") that already has an active Connection" << eol;
            sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
        } else {
            logger << logDebug << "Attempting to re-establish the Session for Client '" << clientID
                   << "'" << eol;
            const bool cleanSession = connectMessage.cleanSession();
            session->reconnect(cleanSession, connection, keepAliveTime);
            connection->connectTo(session);
            sendMQTTConnectAckMessage(connection, !cleanSession, MQTT_CONNACK_ACCEPTED);
        }
    } else {
        session = findAvailableSession();
        if (session) {
            logger << logDebug << "Starting new MQTT Session for Client '" << clientID << "'"
                   << eol;
            session->begin(connectMessage.cleanSession(), clientID, connection, keepAliveTime);
            connection->connectTo(session);
            logger << logDebug << "MQTT Client '" << clientID << "' connected with new Session"
                   << eol;
            sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_ACCEPTED);
        } else {
            logger << logWarning << "MQTT CONNECT with Sessions full. Client ID " << clientID
                   << " refused." << eol;
            sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
        }
    }
}

MQTTSession *MQTTBroker::findMatchingSession(const char *clientID) {
    unsigned sessionIndex;
    for (sessionIndex = 0; sessionIndex < maxMQTTSessions; sessionIndex++) {
        if (sessionValid[sessionIndex]) {
            MQTTSession &session = sessions[sessionIndex];
            if (session.matches(clientID)) {
                return &session;
            }
        }
    }

    return NULL;
}

MQTTSession *MQTTBroker::findAvailableSession() {
    unsigned sessionIndex;
    for (sessionIndex = 0; sessionIndex < maxMQTTSessions; sessionIndex++) {
        if (!sessionValid[sessionIndex]) {
            sessionValid[sessionIndex] = true;
            return &sessions[sessionIndex];
        }
    }

    return NULL;
}

void MQTTBroker::subscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
    logger << logDebug << "Processing Subscribe message" << eol;

    if (!connection->hasSession()) {
        logger << logWarning << "Received a Subscribe message from an unconnected Client ("
               << connection->ipAddress() << ":" << connection->port()
               << "). Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }
    MQTTSession *session = connection->session();

    MQTTSubscribeMessage subscribeMessage(message);
    if (!subscribeMessage.parse()) {
        logger << logWarning << "Bad subscribe message from Client '" << session->name()
               << "'. Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }

    session->resetKeepAliveTimer();

    // Loop through the topics, trying to subscribe to each and adding the result to the SUBACK
    // message.
    unsigned topicFilterCount = subscribeMessage.numTopicFilters();
    uint8_t *subscribeResults = new uint8_t[topicFilterCount];
    unsigned topicFilterIndex;
    for (topicFilterIndex = 0; topicFilterIndex < topicFilterCount; topicFilterIndex++) {
        MQTTString *topicFilterStr;
        uint8_t maxQoS;
        if (!subscribeMessage.getTopicFilter(topicFilterStr, maxQoS)) {
            logger << logError << "MQTT SUBSCRIBE has a messed up number of topic filters" << eol;
            break;
        }

        logger << logDebug << "MQTT Client '" << session->name() << "' wants to subscribe to '"
               << *topicFilterStr << "' with max QoS " << maxQoS << eol;

        char topicFilter[maxTopicFilterLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicFilterLength)) {
            logger << logWarning << "MQTT SUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
            subscribeResults[topicFilterIndex] = mqttSubscribeResult(false, 0);
        } else {
            if (dataModel.subscribe(topicFilter, *session, (uint32_t)maxQoS)) {
                logger << logDebug << "Topic Filter '" << topicFilter << "' subscribed to by '"
                       << session->name() << "'" << eol;
                subscribeResults[topicFilterIndex] = mqttSubscribeResult(true, 0);
            } else {
                logger << logDebug << "Client '" << session->name()
                       << "' failed to subscribe to Topic Filter '" << topicFilter << "'" << eol;
                 subscribeResults[topicFilterIndex] = mqttSubscribeResult(false, 0);
           }
        }
    }

    logger << logDebug << "Sending SUBACK message with " << topicFilterCount
           << " results to Client '" << session->name() << "'" << eol;
    if (!sendMQTTSubscribeAckMessage(connection, subscribeMessage.packetId(), topicFilterCount,
                                     subscribeResults)) {
        logger << logError << "Failed to send SUBACK message to Client '" << session->name() << "'"
               << eol;
    }

    free(subscribeResults);
}

void MQTTBroker::unsubscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
    logger << logDebug << "Processing Unsubscribe message" << eol;

    if (!connection->hasSession()) {
        logger << logWarning << "Received an unsubscribe message from an unconnected Client ("
               << connection->ipAddress() << ":" << connection->port()
               << "). Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }
    MQTTSession *session = connection->session();

    MQTTUnsubscribeMessage unsubscribeMessage(message);
    if (!unsubscribeMessage.parse()) {
        logger << logWarning << "Bad unsubscribe message from Client '" << session->name()
               << "'. Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }

    session->resetKeepAliveTimer();

    unsigned topicFilterCount = unsubscribeMessage.numTopicFilters();
    unsigned topicFilterIndex;
    for (topicFilterIndex = 0; topicFilterIndex < topicFilterCount; topicFilterIndex++) {
        MQTTString *topicFilterStr;
        if (!unsubscribeMessage.getTopicFilter(topicFilterStr)) {
            logger << logError << "MQTT UNSUBSCRIBE has a messed up number of topic filters" << eol;
            break;
        }

        logger << logDebug << "MQTT Client '" << session->name() << "' wants to unsubscribe from '"
               << *topicFilterStr << "'" << eol;

        char topicFilter[maxTopicFilterLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicFilterLength)) {
            logger << logWarning << "MQTT UNSUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
        } else {
            dataModel.unsubscribe(topicFilter, *session);
            logger << logDebug << "Topic Filter '" << topicFilter << "' unsubscribed from by '"
                   << session->name() << "'" << eol;
        }
    }

    logger << logDebug << "Sending UNSUBACK message to Client '" << session->name() << "'" << eol;
    if (!sendMQTTUnsubscribeAckMessage(connection, unsubscribeMessage.packetId())) {
        logger << logError << "Failed to send UNSUBACK message to Client '" << session->name()
               << "'" << eol;
    }
}

void MQTTBroker::pingRequestMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
    MQTTPingRequestMessage pingRequestMessage(message);

    if (!pingRequestMessage.parse()) {
        logger << logError << "Bad MQTT PINGREQ message. Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }

    // Flag if we're getting a PINGREQ for a connection that didn't actually get connected with a
    // session.
    if (!connection->hasSession()) {
        logger << logError
               << "Received MQTT PINGREQ message for a connection that wasn't connected to a "
                  "session."
               << eol;
        terminateConnection(connection);
        return;
    }

    MQTTSession *session = connection->session();
    session->resetKeepAliveTimer();

    logger << logError << "Sending MQTT PINGRESP message to Client '" << session->name() << eol;

    if (!sendMQTTPingResponseMessage(connection)) {
        logger << logError << "Failed to sent MQTT PINGRESP message to "
               << connection->ipAddress() << ":" << connection->port() << eol;
    }
}

void MQTTBroker::disconnectMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
    MQTTDisconnectMessage disconnectMessage(message);

    // We do this for the log message, the connection is going the way of the water buffalo either
    // way.
    if (!disconnectMessage.parse()) {
        logger << logError << "Bad MQTT DISCONNECT message. Terminating connection." << eol;
        terminateConnection(connection);
        return;
    }

    // Flag if we're getting a DISCONNECT for a connection that didn't actually get connected with a
    // session.
    if (!connection->hasSession()) {
        logger << logError
               << "Received MQTT DISCONNECT message for a connection that wasn't connected to a "
                  "session."
               << eol;
        terminateConnection(connection);
        return;
    }

    logger << logDebug << "Stopping client due to DISCONNECT" << eol;
    terminateConnection(connection);
}

void MQTTBroker::reservedMsgReceivedError(MQTTConnection *connection, MQTTMessage &message) {
    logger << logError << "Received reserved message " << message.messageTypeStr()
           << ". Terminating connection" << eol;
    terminateConnection(connection);
}

void MQTTBroker::serverOnlyMsgReceivedError(MQTTConnection *connection, MQTTMessage &message) {
    logger << logError << "Received server->client only message " << message.messageTypeStr()
           << ". Terminating connection" << eol;
    terminateConnection(connection);
}