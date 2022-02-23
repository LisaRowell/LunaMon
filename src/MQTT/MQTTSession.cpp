#include "MQTTSession.h"
#include "MQTTPublishMessage.h"
#include "MQTTBroker.h"

#include "DataModel/DataModel.h"

#include "Util/Logger.h"

#include <stdint.h>

bool MQTTSession::isConnected() const {
    return connection != nullptr;
}

bool MQTTSession::matches(const char *clientID) const {
    return strcmp(clientID, MQTTSession::clientID) == 0;
}

void MQTTSession::begin(bool cleanSession, const char *clientID, MQTTConnection *connection,
                        uint16_t keepAliveTime) {
    MQTTSession::cleanSession = cleanSession;
    strcpy(MQTTSession::clientID, clientID);
    MQTTSession::connection = connection;

    MQTTSession::keepAliveTime = keepAliveTime;
    resetKeepAliveTimer();

    logger << logDebugMQTT << "Established new session for Client ID '" << clientID
           << "' with a keep alive of " << keepAliveTime << " sec"  << eol;
}

void MQTTSession::reconnect(bool newCleanSession, MQTTConnection *connection,
                            uint16_t keepAliveTime) {
    // If the client reconnects with the Clean Session set, we need to clear out the old
    // subscriptions before we establish the connection. This typically happens when a client that
    // wanted us to maintain state across disconnects decides to shutdown the session.
    if (!newCleanSession && cleanSession) {
        unsubscribeAll();
    }

    cleanSession = newCleanSession;

    this->keepAliveTime = keepAliveTime;
    resetKeepAliveTimer();

    MQTTSession::connection = connection;
}

void MQTTSession::service(MQTTBroker *broker) {
    // Do things like timeout sessions whose connection died and hasn't returned.
    if (isConnected()) {
        if (keepAliveTimer.expired()) {
            logger << logDebugMQTT << "Keep alive time expired for Client '" << clientID
                   << "'. Disconnecting..." << eol;
            broker->terminateConnection(connection);
        }
    } else {
        if (tearDownTimer.expired()) {
            logger << logDebugMQTT << "Client '" << clientID
                   << "' failed to reconnect in the allotted time. Terminating Session" << eol;
            dataModel.unsubscribeAll(*this);
            broker->terminateSession(this);
        }
    }
}

void MQTTSession::resetKeepAliveTimer() {
    keepAliveTimer.setSeconds(keepAliveTime + keepAliveTime / 2);
}

// Returns true if the client is to be retained in hopes of the connection being reestablished.
bool MQTTSession::disconnect() {
    connection = nullptr;

    // If the connection was established with Clean Session set, then we don't retain state for the
    // client. Unsubscribe from all connections and return false indicating that the broker should
    // free up the connection.
    if (cleanSession) {
        unsubscribeAll();
        return false;
    } else {
        // We'll keep the session around for a bit, waiting for the client to reconnect...
        tearDownTimer.setSeconds(unconnectedSessionTearDownTime);
        return true;
    }
}

void MQTTSession::unsubscribeAll() {
    dataModel.unsubscribeAll(*this);
}

const char *MQTTSession::name() const {
    return clientID;
}

void MQTTSession::publish(const char *topic, const char *value, bool retainedValue) {
    logger << logDebugMQTT << "Publishing Topic '" << topic << "' to Client '" << clientID
           << "' with value '" << value << "' and retain " << retainedValue << eol;
    
    sendMQTTPublishMessage(connection, topic, value, false, 0, retainedValue, 0);
}

void MQTTSession::updateSessionDebug(DataModelStringLeaf *debug) {
    char sessionDebug[maxSessionDescriptionLength];
    if (connection) {
        char connectionIPAddressStr[maxIPAddressTextLength];
        ipAddressToStr(connectionIPAddressStr, connection->ipAddress());

        snprintf(sessionDebug, maxSessionDescriptionLength, "%s (%s:%u)",
                 clientID, connectionIPAddressStr, connection->port());
    } else {
        snprintf(sessionDebug, maxSessionDescriptionLength, "%s", clientID);
    }

    if (strcmp(sessionDebug, *debug) != 0) {
        *debug = sessionDebug;
    }
}
