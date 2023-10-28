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

#include "MQTTBroker.h"
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
#include "MQTTPublishMessage.h"
#include "MQTTUtil.h"

#include "DataModel/DataModel.h"

#include "StatsManager/StatsManager.h"

#include "Util/StringTools.h"
#include "Util/Error.h"
#include "Util/Logger.h"

#include "WiFiManager/WiFiManager.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

MQTTBroker::MQTTBroker(StatsManager &statsManager)
        : wifiIsConnected(false), wifiServer(portNumber) {
    statsManager.addStatsHolder(this);

    unsigned connectionIndex;
    for (connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        connectionValid[connectionIndex] = false;
    }

    unsigned sessionPos;
    for (sessionPos = 0; sessionPos < maxMQTTSessions; sessionPos++) {
        sessionValid[sessionPos] = false;
    }

    dataModelDebugNeedsUpdating = true;

    sysBrokerClientsConnected = 0;
    sysBrokerClientsDisconnected = 0;
    sysBrokerClientsMaximum = 0;
    sysBrokerClientsTotal = 0;

    messagesReceived = 0;
    sysBrokerMessagesReceived = 0;
    messagesSent = 0;
    sysBrokerMessagesSent = 0;
    publishMessagesReceived = 0;
    sysBrokerMessagesReceived = 0;
    publishMessagesSent = 0;
    sysBrokerMessagesSent = 0;
    publishMessagesDropped = 0;
    sysBrokerMessagesPublishDropped = 0;
}

void MQTTBroker::begin(WiFiManager &wifiManager) {
    wifiManager.registerForNotifications(this);
}

void MQTTBroker::service() {
    checkForLostConnections();
    serviceSessions();
    if (dataModelDebugNeedsUpdating) {
        updateDataModelDebugs();
    }

    if (wifiIsConnected) {
        serviceConnections();

        bool newClientRead;
        do {
            // The WIFI NINA implementation as of version 1.8.13 has a very poor implementation
            // of WiFiServer::available which will return the last client used if it happens to
            // have data available, or, if not, a new client that was connect and has data
            // available. It's as if this was implementing a combination of an accept and polling
            // on a socket. The issue is if we rely on it for socket servicing we starve whatever
            // socket wasn't last and if we service the sockets first then call available(), there
            // is a race condition where new data arriving could still get us an old socket returned
            // as if it's new. Additionally, if we have a client close, and the socket happens to
            // get reused by some other WiFi client, such as the NMEA WiFi source, we could get
            // back a WiFiClient from available() which isn't even an MQTT client at all. Sigh.

            // Until the code is fixed (aka rewritten), we work around this problem by polling the
            // previously connected WiFiClients first, to prevent an overlooked socket, then
            // checking the WiFiClient received back from available() against existing clients on
            // the WiFi to make sure what we get back is truely new.
            // 
            WiFiClient wifiClient = wifiServer.available();
            if (wifiClient) {
                // We check here if the WiFiClient returned is an existing MQTT connection, but
                // really we need to check if it's ANY existing WiFi client, including one for the
                // NMEA over WiFi feed.
                if (!wifiClientIsExistingConnection(wifiClient)) {
                    handleNewWiFiClient(wifiClient);
                    newClientRead = true;
                } else {
                    // The client is some existing WiFi client that happens to have been read last
                    // and has data and is not actually new. Ignore it and let normal connection
                    // servicing get out around the problem...hopefully.
                    newClientRead = false;
                }
            } else {
                newClientRead = false;
            }
        } while (newClientRead);
    }
}

void MQTTBroker::serviceConnections() {
    unsigned connectionPos;
    for (connectionPos = 0; connectionPos < maxMQTTSessions; connectionPos++) {
        if (connectionValid[connectionPos]) {
            MQTTConnection *connection = &connections[connectionPos];
            serviceConnection(connection);
        }
    }
}

void MQTTBroker::exportStats(uint32_t msElapsed) {
    sysBrokerMessagesReceived = messagesReceived;
    sysBrokerMessagesSent = messagesSent;
    sysBrokerMessagesPublishReceived = publishMessagesReceived;
    sysBrokerMessagesPublishSent = publishMessagesSent;
    sysBrokerMessagesPublishDropped = publishMessagesDropped;
}

void MQTTBroker::handleNewWiFiClient(WiFiClient &wifiClient) {
    MQTTConnection *connection = newConnection(wifiClient);
    if (connection == NULL) {
        refuseIncomingWiFiClient(wifiClient);
        return;
    }
    logger << logNotify << "Established MQTT Connection from " << connection->ipAddress() << ":"
           << connection->port() << eol;

    serviceConnection(connection);
}

void MQTTBroker::serviceConnection(MQTTConnection *connection) {
    // We loop here, getting all available messages out of the socket so that we reduce the risk
    // of hitting the WiFi::available() bug where an existing connection is returned instead of
    // a new connection. If we do hit it, we'll end up back here pulling the new message data and
    // hopefully we won't start the new connection.
    bool messageRead;
    do {
        bool errorTerminateConnection;
        MQTTMessage message;
        messageRead = connection->readMessageData(message, errorTerminateConnection);
        if (errorTerminateConnection) {
            terminateConnection(connection);
            return;
        }
        if (messageRead) {
            messageReceived(connection, message);
        }
    } while(messageRead);
}

void MQTTBroker::publishToConnection(MQTTConnection *connection, etl::istring &clientID,
                                 const char *topic, const char *value, bool retainedValue) {
    logger << logDebugMQTT << "Publishing Topic '" << topic << "' to Client '" << clientID
           << "' with value '" << value << "' and retain " << retainedValue << eol;

    if (connection) {
        sendMQTTPublishMessage(connection, topic, value, false, 0, retainedValue, 0);
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
                    dataModelDebugNeedsUpdating = true;
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
            dataModelDebugNeedsUpdating = true;
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

bool MQTTBroker::wifiClientIsExistingConnection(WiFiClient &wifiClient) {
    unsigned connectionIndex;
    for (connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        if (connectionValid[connectionIndex]) {
            MQTTConnection *connection = &connections[connectionIndex];
            if (connection->matches(wifiClient)) {
                return true;
            }
        }
    }

    return false;
}

MQTTConnection *MQTTBroker::newConnection(WiFiClient &wifiClient) {
    unsigned connectionIndex;
    for (connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        if (!connectionValid[connectionIndex]) {
            MQTTConnection *connection = &connections[connectionIndex];
            connection->begin(wifiClient);
            connectionValid[connectionIndex] = true;
            dataModelDebugNeedsUpdating = true;
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
                dataModelDebugNeedsUpdating = true;
            }
        }
    }
}

void MQTTBroker::cleanupLostConnection(MQTTConnection &connection) {
    logger << logDebugMQTT << "Lost TCP connection from " << connection.ipAddress() << ":"
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
            dataModelDebugNeedsUpdating = true;
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
            session.service();
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
    messagesReceived++;

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

        case MQTT_MSG_PUBREC:
            publishMessagesReceived++;
            logger << logWarning << "Received unimplemented message type "
                   << message.messageTypeStr() << eol;
            break;

        case MQTT_MSG_PUBACK:
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
    etl::string<maxMQTTClientIDLength> clientID;
    if (!clientIDStr->copyTo(clientID)) {
        logger << logWarning << "MQTT CONNECT message with too long of a Client ID:" << *clientIDStr
               << eol;
        sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED);
        return;
    }

    // It's less than helpful to have a client connecting without providing a Client ID, though it
    // is allowed by the spec iff the clean session flag is set. We give such connections a name
    // based on their IP address and TCP port so that we can better debug things. It's best to
    // configure the offending application and have it provide sonmething useful...
    if (clientID.empty()) {
        etl::string_stream clientIDStream(clientID);
    
        char connectionIPAddressStr[maxIPAddressTextLength];
        ipAddressToStr(connectionIPAddressStr, connection->ipAddress());

        clientIDStream << connectionIPAddressStr << ":" << connection->port();
    }

    uint16_t keepAliveTime = connectMessage.keepAliveSec();

    MQTTSession *session = findMatchingSession(clientID);
    if (session) {
        if (session->isConnected()) {
            logger << logWarning << "MQTT CONNECT message received for a Client ID (" << clientID
                   << ") that already has an active Connection" << eol;
            sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
        } else {
            logger << logDebugMQTT << "Attempting to re-establish the Session for Client '"
                   << clientID << "'" << eol;
            const bool cleanSession = connectMessage.cleanSession();
            session->reconnect(cleanSession, connection, keepAliveTime);
            connection->connectTo(session);
            if (sendMQTTConnectAckMessage(connection, !cleanSession, MQTT_CONNACK_ACCEPTED)) {}
            dataModelDebugNeedsUpdating = true;
        }
    } else {
        session = findAvailableSession();
        if (session) {
            logger << logDebugMQTT << "Starting new MQTT Session for Client '" << clientID << "'"
                   << eol;
            session->begin(this, connectMessage.cleanSession(), clientID, connection,
                           keepAliveTime);
            connection->connectTo(session);
            logger << logDebugMQTT << "MQTT Client '" << clientID << "' connected with new Session"
                   << eol;
            sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_ACCEPTED);
            dataModelDebugNeedsUpdating = true;
        } else {
            logger << logWarning << "MQTT CONNECT with Sessions full. Client ID " << clientID
                   << " refused." << eol;
            sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
        }
    }
}

MQTTSession *MQTTBroker::findMatchingSession(const etl::istring &clientID) {
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

        logger << logDebugMQTT << "MQTT Client '" << session->name() << "' wants to subscribe to '"
               << *topicFilterStr << "' with max QoS " << maxQoS << eol;

        char topicFilter[maxTopicFilterLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicFilterLength)) {
            logger << logWarning << "MQTT SUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
            subscribeResults[topicFilterIndex] = mqttSubscribeResult(false, 0);
        } else {
            if (dataModel.subscribe(topicFilter, *session, (uint32_t)maxQoS)) {
                logger << logDebugMQTT << "Topic Filter '" << topicFilter << "' subscribed to by '"
                       << session->name() << "'" << eol;
                subscribeResults[topicFilterIndex] = mqttSubscribeResult(true, 0);
            } else {
                logger << logWarning << "Client '" << session->name()
                       << "' failed to subscribe to Topic Filter '" << topicFilter << "'" << eol;
                 subscribeResults[topicFilterIndex] = mqttSubscribeResult(false, 0);
           }
        }
    }

    logger << logDebugMQTT << "Sending SUBACK message with " << topicFilterCount
           << " results to Client '" << session->name() << "'" << eol;
    if (!sendMQTTSubscribeAckMessage(connection, subscribeMessage.packetId(), topicFilterCount,
                                     subscribeResults)) {
        logger << logError << "Failed to send SUBACK message to Client '" << session->name() << "'"
               << eol;
    }

    free(subscribeResults);
}

void MQTTBroker::unsubscribeMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
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

        logger << logDebugMQTT << "MQTT Client '" << session->name()
               << "' wants to unsubscribe from '" << *topicFilterStr << "'" << eol;

        char topicFilter[maxTopicFilterLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicFilterLength)) {
            logger << logWarning << "MQTT UNSUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
        } else {
            dataModel.unsubscribe(topicFilter, *session);
            logger << logDebugMQTT << "Topic Filter '" << topicFilter << "' unsubscribed from by '"
                   << session->name() << "'" << eol;
        }
    }

    logger << logDebugMQTT << "Sending UNSUBACK message to Client '" << session->name() << "'"
           << eol;
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

    logger << logDebugMQTT << "Sending MQTT PINGRESP message to Client '" << session->name() << eol;

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

    logger << logDebugMQTT << "Stopping client due to DISCONNECT" << eol;
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

bool MQTTBroker::sendMQTTConnectAckMessage(MQTTConnection *connection, bool sessionPresent,
                                           uint8_t returnCode) {
    MQTTFixedHeader fixedHeader;
    MQTTConnectAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_CONNACK << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTConnectAckVariableHeader);
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    variableHeader.flags = 0;
    if (sessionPresent) {
        variableHeader.flags |= MQTT_CONNACK_SESSION_PRESENT_MASK;
    }
    variableHeader.returnCode = returnCode;

    if (!connection->write((uint8_t *)&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    messagesSent++;

    return true;
}

bool MQTTBroker::sendMQTTPingResponseMessage(MQTTConnection *connection) {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PINGRESP << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }

    if (!mqttWriteRemainingLength(connection, 0)) {
        return false;
    }

    messagesSent++;

    return true;
}

bool MQTTBroker::sendMQTTPublishMessage(MQTTConnection *connection, const char *topic,
                                        const char *value, bool dup, uint8_t qosLevel,
                                        bool retain, uint16_t packetId) {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PUBLISH << MQTT_MSG_TYPE_SHIFT;
    if (dup) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_DUP_MASK;
    }
    fixedHeader.typeAndFlags |= qosLevel << MQTT_PUBLISH_FLAGS_QOS_SHIFT;
    if (retain) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_RETAIN_MASK;
    }
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        publishMessagesDropped++;
        return false;
    }

    const size_t valueLength = strlen(value);
    uint32_t remainingLength;
    remainingLength = strlen(topic) + 2 + valueLength;
    if (qosLevel > 0) {
        remainingLength += 2;
    }
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        publishMessagesDropped++;
        return false;
    }

    if (!mqttWriteMQTTString(connection, topic)) {
        publishMessagesDropped++;
        return false;
    }

    if (qosLevel > 0) {
        if (!mqttWriteUInt16(connection, packetId)) {
            publishMessagesDropped++;
            return false;
        }
    }

    if (!connection->write((uint8_t *)value, valueLength)) {
        publishMessagesDropped++;
        return false;
    }

    messagesSent++;
    publishMessagesSent++;

    return true;
}

bool MQTTBroker::sendMQTTSubscribeAckMessage(MQTTConnection *connection, uint16_t packetId,
                                             uint8_t numberResults, uint8_t *results) {
    MQTTFixedHeader fixedHeader;
    MQTTSubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_SUBACK << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTSubscribeAckVariableHeader) + numberResults;
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (!connection->write((uint8_t *)&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    if (!connection->write(results, numberResults * sizeof(uint8_t))) {
        return false;
    }

    messagesSent++;

    return true;
}

uint8_t MQTTBroker::mqttSubscribeResult(bool success, uint8_t maxQoS) {
    if (!success) {
        return MQTT_SUBACK_FAILURE_FLAG;
    } else {
        return maxQoS;
    }
}

bool MQTTBroker::sendMQTTUnsubscribeAckMessage(MQTTConnection *connection, uint16_t packetId) {
    MQTTFixedHeader fixedHeader;
    MQTTUnsubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_UNSUBACK << MQTT_MSG_TYPE_SHIFT;
    if (!connection->write((uint8_t *)&fixedHeader, sizeof(fixedHeader))) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTUnsubscribeAckVariableHeader);
    if (!mqttWriteRemainingLength(connection, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (!connection->write((uint8_t *)&variableHeader, sizeof(variableHeader))) {
        return false;
    }

    messagesSent++;

    return true;
}

void MQTTBroker::updateDataModelDebugs() {
    updateConnectionDebugs();
    updateSessionDebugs();
}

void MQTTBroker::updateConnectionDebugs() {
    unsigned connectionDebugPos = 0;
    for (unsigned connectionIndex = 0; connectionIndex < maxMQTTSessions; connectionIndex++) {
        if (connectionValid[connectionIndex]) {
            MQTTConnection &connection = connections[connectionIndex];
            DataModelStringLeaf &connectionDebug = *sysBrokerConnectionDebugs[connectionDebugPos];
            connection.updateConnectionDebug(connectionDebug);
            connectionDebugPos++;
        }
    }
    for (; connectionDebugPos < maxMQTTSessions; connectionDebugPos++) {
        DataModelStringLeaf &emptyConnectionDebug = *sysBrokerConnectionDebugs[connectionDebugPos];
        emptyConnectionDebug = "";
    }
}

void MQTTBroker::updateSessionDebugs() {
    uint32_t connectedClients = 0;
    uint32_t disconnectedClients = 0;

    unsigned sessionDebugPos = 0;
    for (unsigned sessionIndex = 0; sessionIndex < maxMQTTSessions; sessionIndex++) {
        if (sessionValid[sessionIndex]) {
            MQTTSession &session = sessions[sessionIndex];
            DataModelStringLeaf &sessionDebug = *sysBrokerSessionDebugs[sessionDebugPos];
            session.updateSessionDebug(sessionDebug);
            sessionDebugPos++;

            if (session.isConnected()) {
                connectedClients++;
            } else {
                disconnectedClients++;
            }
        }
    }
    for (; sessionDebugPos < maxMQTTSessions; sessionDebugPos++) {
        DataModelStringLeaf &emptySessionDebug = *sysBrokerSessionDebugs[sessionDebugPos];
        emptySessionDebug = "";
    }

    sysBrokerClientsConnected = connectedClients;
    sysBrokerClientsDisconnected = disconnectedClients;
    if (connectedClients > sysBrokerClientsMaximum) {
        sysBrokerClientsMaximum = connectedClients;
    }
    const uint32_t totalClients = connectedClients + disconnectedClients;
    sysBrokerClientsTotal = totalClients;

    dataModelDebugNeedsUpdating = false;
}
