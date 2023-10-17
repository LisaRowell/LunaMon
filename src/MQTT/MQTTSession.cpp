#include "MQTTSession.h"
#include "MQTTPublishMessage.h"
#include "MQTTBroker.h"

#include "DataModel/DataModel.h"

#include "Util/Logger.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

bool MQTTSession::isConnected() const {
    return connection != nullptr;
}

bool MQTTSession::matches(const etl::istring &clientID) const {
    return this->clientID.compare(clientID) == 0;
}

void MQTTSession::begin(MQTTBroker *broker, bool cleanSession, const etl::istring &clientID,
                        MQTTConnection *connection, uint16_t keepAliveTime) {
    this->broker = broker;
    this->cleanSession = cleanSession;
    this->clientID = clientID;
    this->connection = connection;

    this->keepAliveTime = keepAliveTime;
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

void MQTTSession::service() {
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

const etl::istring &MQTTSession::name() const {
    return clientID;
}

void MQTTSession::publish(const char *topic, const char *value, bool retainedValue) {
    if (connection) {
        broker->publishToConnection(connection, clientID, topic, value, retainedValue);
    }
}

void MQTTSession::updateSessionDebug(DataModelStringLeaf &debug) {
    etl::string<maxSessionDescriptionLength> sessionDebug;
    etl::string_stream sessionDebugStream(sessionDebug);

    if (connection) {
        char connectionIPAddressStr[maxIPAddressTextLength];
        ipAddressToStr(connectionIPAddressStr, connection->ipAddress());

        sessionDebugStream << clientID << " (" << connectionIPAddressStr << ":"
                           << connection->port() << ")";
    } else {
        sessionDebugStream << clientID;
    }

    debug = sessionDebug;
}
