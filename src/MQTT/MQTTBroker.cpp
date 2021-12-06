#include <Arduino.h>

#include "MQTTBroker.h"
#include "WiFiManager/WiFiManager.h"
#include "MQTTConnection.h"
#include "MQTTClient.h"
#include "MQTTMessage.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTDisconnectMessage.h"
#include "Util/Error.h"

MQTTBroker::MQTTBroker() : wifiIsConnected(false), wifiServer(portNumber) {
  unsigned clientPos;
  for (clientPos = 0; clientPos < maxMQTTClients; clientPos++) {
    clientValid[clientPos] = false;
  }
}

void MQTTBroker::begin(WiFiManager &wifiManager) {
  wifiManager.registerForNotifications(this);
}

void MQTTBroker::service() {
  checkForLostConnections();
  serviceClients();

  if (wifiIsConnected) {
    bool clientWasAvailable;
    do {
      WiFiClient wifiClient = wifiServer.available();
      if (wifiClient) {
        serviceWiFiClientWithData(wifiClient);
        clientWasAvailable = true;
      } else {
        clientWasAvailable = false;
      }
    } while (clientWasAvailable);
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

  if (connection->hasMQTTClient()) {
    MQTTClient *client = connection->client();
    bool retain = client->disconnect();
    if (!retain) {
      unsigned clientPos;
      bool clientFound;
      for (clientPos = 0, clientFound = false; clientPos < maxMQTTClients && !clientFound; clientPos++) {
        if (clientValid[clientPos] && &clients[clientPos] == client) {
          clientFound = true;
          clientValid[clientPos] = false;
        }
      }

      if (!clientFound) {
        fatalError("Lost track of a MQTT Session and couldn't delete it");
      }
    }
  }

  unsigned connectionPos;
  for (connectionPos = 0; connectionPos < maxMQTTClients; connectionPos++) {
    if (&connections[connectionPos] == connection) {
      connectionValid[connectionPos] = false;
      // Need to add code to deal with a connected MQTT Client.
      return;
    }
  }

  fatalError("Lost track of an MQTT connection");
}

MQTTConnection *MQTTBroker::findExistingConnection(WiFiClient &wifiClient) {
  unsigned connectionIndex;
  for (connectionIndex = 0; connectionIndex < maxMQTTClients; connectionIndex++) {
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
  for (connectionIndex = 0; connectionIndex < maxMQTTClients; connectionIndex++) {
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
  for (connectionIndex = 0; connectionIndex < maxMQTTClients; connectionIndex++) {
    if (connectionValid[connectionIndex]) {
      MQTTConnection &connection = connections[connectionIndex];
      if (connection.wasDisconnected()) {
        MQTTClient *mqttClient = connection.client();
        bool retainConnection = mqttClient->disconnect();
        if (!retainConnection) {
          invalidateClient(mqttClient);
        }
        connectionValid[connectionIndex] = false;
      }
    }
  }
}

void MQTTBroker::invalidateClient(MQTTClient *client) {
  unsigned clientIndex;
  for (clientIndex = 0; clientIndex < maxMQTTClients; clientIndex++) {
    if (clientValid[clientIndex] && &clients[clientIndex] == client) {
      clientValid[clientIndex] = false;
      return;
    }
  }

  fatalError("Lost track of a MQTT client and couldn't invalidate it.");
}

void MQTTBroker::serviceClients() {
  unsigned clientIndex;

  for (clientIndex = 0; clientIndex < maxMQTTClients; clientIndex++) {
    if (clientValid[clientIndex]) {
      MQTTClient &client = clients[clientIndex];
      client.service();
    }
  }
}

void MQTTBroker::refuseIncomingWiFiClient(WiFiClient &wifiClient) {
  Serial.print("Maximum number of MQTT WiFi clients exceeded, refusing incoming connection from ");
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
  if (connection->hasMQTTClient()) {
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

  MQTTClient *client = findMatchingSession(clientID);
  if (client) {
    if (client->isConnected()) {
      Serial.print("MQTT CONNECT message received for a Client ID (");
      Serial.print(clientID);
      Serial.println(") that already has an active Connection");
      sendMQTTConnectAckMessage(connection, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
    } else {
      const bool cleanSession = connectMessage.cleanSession();
      client->reconnect(cleanSession, connection);
      connection->connectTo(client);
      sendMQTTConnectAckMessage(connection, !cleanSession, MQTT_CONNACK_ACCEPTED);
    }
  } else {
    client = findAvailableSession();
    if (client) {
      client->begin(connectMessage.cleanSession(), clientID, connection);
      connection->connectTo(client);
      Serial.print("MQTT Client '");
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

MQTTClient *MQTTBroker::findMatchingSession(const char *clientID) {
  unsigned sessionIndex;
  for (sessionIndex = 0; sessionIndex < maxMQTTClients; sessionIndex++) {
    if (clientValid[sessionIndex]) {
      MQTTClient &session = clients[sessionIndex];
      if (session.matches(clientID)) {
        return &session;
      }
    }
  }

  return NULL;
}

MQTTClient *MQTTBroker::findAvailableSession() {
  unsigned sessionIndex;
  for (sessionIndex = 0; sessionIndex < maxMQTTClients; sessionIndex++) {
    if (!clientValid[sessionIndex]) {
      clientValid[sessionIndex] = true;
      return &clients[sessionIndex];
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
  if (!connection->hasMQTTClient()) {
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