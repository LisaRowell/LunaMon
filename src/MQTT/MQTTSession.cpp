#include <Arduino.h>

#include "MQTTSession.h"
#include "Util/Logger.h"

bool MQTTSession::isConnected() const {
    return connection != NULL;
}

bool MQTTSession::matches(const char *clientID) const {
    return strcmp(clientID, MQTTSession::clientID) == 0;
}

void MQTTSession::begin(bool cleanSession, const char *clientID, MQTTConnection *connection) {
    MQTTSession::cleanSession = cleanSession;
    strcpy(MQTTSession::clientID, clientID);
    MQTTSession::connection = connection;

    logger << logDebug << "Established new session for Client ID '" << clientID << "'" << eol;
}

void MQTTSession::reconnect(bool newCleanSession, MQTTConnection *connection) {
    // If the client reconnects with the Clean Session set, we need to clear out the old
    // subscriptions before we establish the connection. This typically happens when a client that
    // wanted us to maintain state across disconnects decides to shutdown the session.
    if (!newCleanSession && cleanSession) {
        unsubscribeAll();
    }

    cleanSession = newCleanSession;
    MQTTSession::connection = connection;
}

void MQTTSession::service() {
    // Do things like timeout sessions whose connection died and hasn't returned.
}

// Returns true if the client is to be retained in hopes of the connection being reestablished.
bool MQTTSession::disconnect() {
    // Do house keeping like setting a timer to dismantle the client if it's not reconnected in a
    // timely fashion.

    // If the connection was established with Clean Session set, then we don't retain state for the
    // client. Unsubscribe from all connections and return false indicating that the broker should
    // free up the connection.
    if (cleanSession) {
        unsubscribeAll();
        return false;
    } else {
        // Set some sort of a timeout here...
        return true;
    }
}

void MQTTSession::unsubscribeAll() { 
}

const char *MQTTSession::name() const {
    return clientID;
}