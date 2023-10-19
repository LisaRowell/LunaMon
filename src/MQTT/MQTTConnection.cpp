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

#include "MQTTConnection.h"
#include "MQTTBroker.h"
#include "MQTTMessage.h"

#include "DataModel/DataModel.h"

#include "Util/IPAddressTools.h"
#include "Util/Logger.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <WiFiNINA.h>
#include <stdint.h>

void MQTTConnection::begin(WiFiClient &wifiClient) {
    this->wifiClient = wifiClient;
    mqttSession = NULL;
    remoteIPAddress = wifiClient.remoteIP();
    remotePort = wifiClient.remotePort();
    resetMessageBuffer();
}


bool MQTTConnection::matches(WiFiClient &wifiClient) {
    return (this->wifiClient.remoteIP() == wifiClient.remoteIP() &&
            this->wifiClient.remotePort() == wifiClient.remotePort());
}

// Since we're receiving messages over TCP, even if it's a small message we have no guarantee that
// the entire message will be available at this time. We use a buffer in the connection to read in
// data, chunk by chunk, until an entire message is present, then process it. An alternative
// implementation would be to use threads and let the thread block trying to read message data, but
// it's not clear that the WiFiNINA library is thread safe (it's highly unlikely) and we're trying
// to avoid multi-threading.
bool MQTTConnection::readMessageData(MQTTMessage &message, bool &errorTerminateConnection) {
    errorTerminateConnection = false;

    // We check for available data here and store the amount vs in the called and getting it as
    // needed since wifiClient.available() is actually going out over the PCI bus, incurring waits.
    size_t bytesAvailable = (size_t)wifiClient.available();
    if (bytesAvailable == 0) {
        return false;
    }

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
    return wifiClient.write(data, size) == size;
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
    logger << logDebugMQTT << "Stopping client " << wifiClient.remoteIP() << ":"
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

const IPAddress &MQTTConnection::ipAddress() const {
    return remoteIPAddress;
}

uint16_t MQTTConnection::port() const {
    return remotePort;
}

void MQTTConnection::updateConnectionDebug(DataModelStringLeaf &debug) {
    etl::string<maxConnectionDescriptionLength> connectionDebug;
    etl::string_stream connectionDebugStream(connectionDebug);

    char connectionIPAddressStr[maxIPAddressTextLength];
    ipAddressToStr(connectionIPAddressStr, remoteIPAddress);

    connectionDebugStream << connectionIPAddressStr << ":" << remotePort;

    if (hasSession()) {
        connectionDebugStream << " (" << mqttSession->name() << ")";
    }

    debug = connectionDebug;
}
