#include <Arduino.h>
#include <WiFiNINA.h>

#include "MQTTConnection.h"
#include "MQTTMessage.h"

#include "DataModel/DataModel.h"

#include "Util/IPAddressTools.h"
#include "Util/Logger.h"

void MQTTConnection::begin(WiFiClient &wifiClient) {
    this->wifiClient = wifiClient;
    mqttSession = NULL;
    remoteIPAddress = wifiClient.remoteIP();
    remotePort = wifiClient.remotePort();
}

bool MQTTConnection::matches(WiFiClient &wifiClient) {
    return (this->wifiClient.remoteIP() == wifiClient.remoteIP() &&
            this->wifiClient.remotePort() == wifiClient.remotePort());
}

bool MQTTConnection::readMessageData(MQTTMessage &message, bool &errorTerminateConnection) {
    errorTerminateConnection = false;

    // To avoid multi-threading (which the WiFiNINA library would probably bard on),and to avoid
    // twisted state machine code dealing with partial messages, we build up an incoming message in
    // a buffer and when we have a complete one, let the broker deal with it.
    size_t bytesAvailable = (size_t)wifiClient.available();

    // MQTT has a leading header, called the Fixed Header, which has a variable length encoding of
    // the number of bytes coming after the fixed header. This can be used to calculate the overall
    // message length. Since this fixed header isn't actually of fixed length, and instead has a
    // funky, variable length, remaining bytes field, we need to read in the minimum header size,
    // see if it's enough to determine the length, if not, keep reading, byte, by byte, until we
    // know the length.
    while (!messageSizeKnown && bytesAvailable > 0) {
        if (bytesInBuffer < minMQTTFixedHeaderSize) {
            size_t readAmount = min(bytesAvailable, minMQTTFixedHeaderSize);
            readToBuffer(readAmount);
            bytesAvailable -= readAmount;
            if (bytesInBuffer < minMQTTFixedHeaderSize) {
                return false;
            }
        } else {
            readToBuffer(1);
            bytesAvailable--;
        }
        if (messageRemainingLengthIsComplete()) {
            if (!determineMessageLength()) {
                logIllegalRemainingLength();
                errorTerminateConnection = true;
                return false;
            }

            // While the MQTT protocol supports messages of 256 Mb, it's just not practical to
            // support that on an Arduino. We have a fixed length receive buffer; if a client tries
            // to send a message longer than that, thank and excuse the client.
            if (messageSize > maxIncomingMessageSize) {
                logMessageSizeTooLarge();
                errorTerminateConnection = true;
                return false;
            }
        }
    }

    if (messageSizeKnown) {
        const size_t readAmount = min(bytesAvailable, messageSize - bytesInBuffer);
        readToBuffer(readAmount);
        if (bytesInBuffer == messageSize) {
            message = MQTTMessage(buffer, messageSize);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void MQTTConnection::resetMessageBuffer() {
    bytesInBuffer = 0;
    messageSizeKnown = false;
}

// An MQTT message has a variable length field indicating the remaining length of the message after
// the first header. This is encoded as a series of 1-4 bytes with the MSB of the last byte being 0
// with any preceeding bytes having an MSB of 1. This routine requires the caller to call it for
// each byte read in.
bool MQTTConnection::messageRemainingLengthIsComplete() {
    if (bytesInBuffer == maxMQTTFixedHeaderSize) {
        return true;
    } else {
        return (buffer[bytesInBuffer - 1] & 0x80) == 0;
    }
}

bool MQTTConnection::determineMessageLength() {
    uint32_t fixedHeaderSize;
    uint32_t remainingLength;

    fixedHeaderSize = 2;
    remainingLength = buffer[1] & 0x7f;
    if (buffer[1] & 0x80) {
        fixedHeaderSize++;
        remainingLength = remainingLength + (buffer[2] * 128);
        if (buffer[2] & 0x80) {
            fixedHeaderSize++;
            remainingLength = remainingLength + (buffer[3] * 128 * 128);
            if (buffer[3] & 0x80) {
                fixedHeaderSize++;
                remainingLength = remainingLength + (buffer[4] * 128 * 128 * 128);
                if (buffer[4] & 0x80) {
                    // Per the MQTT specification, the last remaining length byte must have a 0 MSB.
                    return false;
                }
            }
        }
    }

    messageSize = fixedHeaderSize + remainingLength;
    messageSizeKnown = true;

    return true;
}

void MQTTConnection::logIllegalRemainingLength() {
    logger << logError <<  "Illegal MQTT message remaining length: " << Hex << buffer[1];
    if (buffer[1] & 0x80) {
        logger << buffer[2];
        if (buffer[2] & 0x80) {
            logger << buffer[3];
            if (buffer[3] & 0x80) {
                logger << buffer[4];
            }
        }
    }
    logger << " Aborting connection " << wifiClient.remoteIP() << ":" << wifiClient.remotePort()
           << eol;
}

void MQTTConnection::logMessageSizeTooLarge() {
    logger << logError << "Message size " << messageSize << " from " << wifiClient.remoteIP()
           << ":" << wifiClient.remotePort() << " exceeds maximum allowable ("
           << maxIncomingMessageSize << "). Aborting connection." << eol;
}

void MQTTConnection::readToBuffer(size_t readAmount) {
    // TODO: Figure out the return on this and interpret accordingly
    wifiClient.read(buffer + bytesInBuffer, readAmount);
    bytesInBuffer += readAmount;
}

bool MQTTConnection::write(const uint8_t *data, size_t size) {
    return wifiClient.write(data, size);
}

bool MQTTConnection::hasSession() {
    return mqttSession != NULL;
}

void MQTTConnection::connectTo(MQTTSession *session) {
    mqttSession = session;
}

MQTTSession *MQTTConnection::session() {
    return mqttSession;
}

void MQTTConnection::stop() {
    logger << logDebug << "Stopping client " << wifiClient.remoteIP() << ":"
           << wifiClient.remotePort() << eol;

    wifiClient.flush();
    wifiClient.stop();
}

bool MQTTConnection::wasDisconnected() {
    // Under the hood, the WiFiNINA driver will close a connection if a call connected() detects
    // that the TCP connection has been closed. We therefore do anything to clean up, like call
    // flush or stop. 
    return !wifiClient.connected();
}

IPAddress MQTTConnection::ipAddress() {
    return remoteIPAddress;
}

uint16_t MQTTConnection::port() {
    return remotePort;
}

void MQTTConnection::updateConnectionDebug(DataModelStringLeaf *debug) {
    char connectionIPAddressStr[maxIPAddressTextLength];
    ipAddressToStr(connectionIPAddressStr, remoteIPAddress);

    char connectionDebug[maxConnectionDescriptionLength];
    if (hasSession()) {
        snprintf(connectionDebug, maxConnectionDescriptionLength, "%s:%u (%s)",
                 connectionIPAddressStr, remotePort, mqttSession->name());
    } else {
        snprintf(connectionDebug, maxConnectionDescriptionLength, "%s:%u", connectionIPAddressStr,
                 remotePort);
    }

    if (strcmp(connectionDebug, *debug) != 0) {
        *debug = connectionDebug;
    }
}
