#include <Arduino.h>

#include "MQTTBroker.h"
#include "WiFiManager/WiFiManager.h"
#include "MQTTConnection.h"
#include "MQTTSession.h"
#include "MQTTMessage.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTDisconnectMessage.h"
#include "Util/Error.h"

MQTTBroker::MQTTBroker() : wifiIsConnected(false), wifiServer(portNumber) {
  unsigned sessionPos;
  for (sessionPos = 0; sessionPos < maxMQTTSessions; sessionPos++) {
    sessionValid[sessionPos] = false;
  }
}

void MQTTBroker::begin(WiFiManager &wifiManager) {
  wifiManager.registerForNotifications(this);
}

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
  // Some client has data for us. Given the wonkiness of the WiFiNINA api,
  // we don't know if this is a new to us TCP connection or one we already
  // know about. We do know that it has data ready to be read.
  MQTTConnection *connection;
  connection = findExistingConnection(wifiClient);
  if (!connection) {
    connection = newConnection(wifiClient);
    if (connection == NULL) {
      refuseIncomingWiFiClient(wifiClient);
    }
  }

  // Since we're receiving messages over TCP, even if it's a small message
  // we have no guarantee that the entire message will be available at this
  // time. We use a buffer in the connection to read in data, chunk by
  // chunk, until an entire message is present, then process it. An
  // alternative implementation would be to use threads and let the thread
  // block trying to read message data, but it's not clear that the
  // WiFiNINA library is thread safe (it's highly unlikely) and we're
  // trying to avoid multi-threading.
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
      for (sessionPos = 0, sessionFound = false; sessionPos < maxMQTTSessions && !sessionFound; sessionPos++) {
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
      // Need to add code to deal with a connected MQTT Session.
      return;
    }
  }

  fatalError("Lost track of an MQTT connection");
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
  Serial.println("Connected to WiFi, starting MQTT server.");
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
        MQTTSession *session = connection.session();
        bool retainConnection = session->disconnect();
        if (!retainConnection) {
          invalidateSession(session);
        }
        connectionValid[connectionIndex] = false;
      }
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
      session.service();
    }
  }
}

void MQTTBroker::refuseIncomingWiFiClient(WiFiClient &wifiClient) {
  Serial.print("Maximum number of MQTT WiFi sessions exceeded, refusing incoming connection from ");
  Serial.print(wifiClient.remoteIP());
  Serial.print(":");
  Serial.println(wifiClient.remotePort());

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
      serverOnlyMsgReceivedError(connection, message);
      break;

    case MQTT_MSG_DISCONNECT:
      disconnectMessageReceived(connection, message);
      break;

    case MQTT_MSG_RESERVED1:
    case MQTT_MSG_RESERVED2:
      reservedMsgReceivedError(connection, message);
      break;

    default:
      Serial.print("Received unimplemented message type ");
      Serial.println(message.messageTypeStr());
  }  

  connection->resetMessageBuffer();
}

void MQTTBroker::connectMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
  // Per the MQTT specification, we try a second CONNECT for a connection as a protocol
  // error.
  if (connection->hasSession()) {
    Serial.println("Second MQTT CONNECT received for a connection.");
    terminateConnection(connection);
  }

  MQTTConnectMessage connectMessage(message);
  if (!connectMessage.parse()) {
    Serial.println("Bad connect message. Terminating connection.");
    terminateConnection(connection);
    return;
  }

  uint8_t errorCode;
  errorCode = connectMessage.sanityCheck();
  if (errorCode != MQTT_CONNACK_ACCEPTED) {
    sendMQTTConnectAckMessage(connection, false, errorCode);
    return;
  }

  // Since this is a light weight broker, and a work in progress, we reject a few currently
  // unsupported types of connections.
  if (connectMessage.hasWill()) {
    Serial.println("MQTT CONNECT with Will: Currently unsupported");
    sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
    return;
  }
  if (connectMessage.hasUserName()) {
    Serial.println("MQTT CONNECT message with Password set");
    sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
    return;
  }
  if (connectMessage.hasPassword()) {
    Serial.println("MQTT CONNECT message with Password set");
    sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
    return;
  }

  const MQTTString *clientIDStr = connectMessage.clientID();
  char clientID[maxMQTTClientIDLength + 1];
  if (!clientIDStr->copyTo(clientID, maxMQTTClientIDLength)) {
    Serial.print("MQTT CONNECT message with too long of a Client ID:");
    clientIDStr->print();
    Serial.println();
    sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED);
    return;
  }

  MQTTSession *session = findMatchingSession(clientID);
  if (session) {
    if (session->isConnected()) {
      Serial.print("MQTT CONNECT message received for a Client ID (");
      Serial.print(clientID);
      Serial.println(") that already has an active Connection");
      sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
    } else {
      const bool cleanSession = connectMessage.cleanSession();
      session->reconnect(cleanSession, connection);
      connection->connectTo(session);
      sendMQTTConnectAckMessage(connection, !cleanSession, MQTT_CONNACK_ACCEPTED);
    }
  } else {
    session = findAvailableSession();
    if (session) {
      session->begin(connectMessage.cleanSession(), clientID, connection);
      connection->connectTo(session);
      Serial.print("MQTT Session '");
      Serial.print(clientID);
      Serial.println("' connected with new Session");
      sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_ACCEPTED);
    } else {
      Serial.print("MQTT CONNECT with Sessions full. Client ID ");
      Serial.print(clientID);
      Serial.println(" refused.");
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

void MQTTBroker::disconnectMessageReceived(MQTTConnection *connection, MQTTMessage &message) {
  MQTTDisconnectMessage disconnectMessage(message);

  // We do this for the log message, the connection is going the way of the water buffalo
  // either way.
  if (!disconnectMessage.parse()) {
    Serial.println("Bad MQTT DISCONNECT message. Terminating connection.");
    terminateConnection(connection);
    return;
  }

  // Flag if we're getting a DISCONNECT for a connection that didn't actually get connected
  // with a session.
  if (!connection->hasSession()) {
    Serial.println("Received MQTT DISCONNECT message for a connection that wasn't connected to a session.");
    terminateConnection(connection);
    return;
  }

  Serial.println("Stopping client due to DISCONNECT");
  terminateConnection(connection);
}

void MQTTBroker::reservedMsgReceivedError(MQTTConnection *connection, MQTTMessage &message) {
  Serial.print("Received reserved message ");
  Serial.print(message.messageTypeStr());
  Serial.println(". Terminating connection");
  terminateConnection(connection);
}

void MQTTBroker::serverOnlyMsgReceivedError(MQTTConnection *connection, MQTTMessage &message) {
  Serial.print("Received server->client only message ");
  Serial.print(message.messageTypeStr());
  Serial.println(". Terminating connection");
  terminateConnection(connection);
}