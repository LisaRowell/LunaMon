#include <Arduino.h>

#include "MQTTClient.h"

bool MQTTClient::isConnected() const {
  return connection != NULL;
}

bool MQTTClient::matches(const char *clientID) const {
  return strcmp(clientID, MQTTClient::clientID) == 0;
}

void MQTTClient::begin(bool cleanSession, const char *clientID, MQTTConnection *connection) {
  MQTTClient::cleanSession = cleanSession;
  strcpy(MQTTClient::clientID, clientID);
  MQTTClient::connection = connection;

  Serial.print("Established new session for Client ID '");
  Serial.print(clientID);
  Serial.println("'");
}

void MQTTClient::reconnect(bool newCleanSession, MQTTConnection *connection) {
  // If the client reconnects with the Clean Session set, we need to clear out
  // the old subscriptions before we establish the connection. This typically
  // happens when a client that wanted us to maintain state across disconnects
  // desides to shutdown the session.
  if (!newCleanSession && cleanSession) {
    unsubscribeAll();
  }

  cleanSession = newCleanSession;
  MQTTClient::connection = connection;
}

void MQTTClient::service() {
  // Do things like timeout clients whose connection died and hasn't returned.
}

// Returns true if the client is to be retained in hopes of the connection being
// reestablished.
bool MQTTClient::disconnect() {
  // Do house keeping like setting a timer to dismantle the client if it's not
  // reconnected in a timely fashion.

  // If the connection was established with Clean Session set, then we don't
  // retain state for the client. Unsubscribe from all connections and return
  // false indicating that the broker should free up the connection.
  if (cleanSession) {
    unsubscribeAll();
    return false;
  } else {
    // Set some sort of a timeout here...
    return true;
  }
}

void MQTTClient::unsubscribeAll() { 
}