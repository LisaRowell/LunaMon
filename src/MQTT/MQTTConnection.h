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

#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H

#include "MQTTMessage.h"

#include "DataModel/DataModelStringLeaf.h"

#include <WiFiNINA.h>
#include <stdint.h>

class MQTTSession;

class MQTTConnection {
    private:
        WiFiClient wifiClient;
        MQTTSession *mqttSession;

        // We cache the client's IP address and TCP port here so that we can use it in debug
        // messages for disconnected clients. This is required because of the roundabout way that
        // we learn about connection termination from the WiFiNINA library. By the time we figure it
        // out, the address and port information has already been smoshed.
        IPAddress remoteIPAddress;
        uint16_t remotePort;

        static const uint32_t maxIncomingMessageSize = 1024;
        uint8_t buffer[maxIncomingMessageSize];
        uint32_t bytesInBuffer;
        bool messageSizeKnown;
        uint32_t messageSize;

        static const size_t minMQTTFixedHeaderSize = 2;
        static const size_t maxMQTTFixedHeaderSize = (minMQTTFixedHeaderSize + 3);

        void readToBuffer(size_t readAmount);
        bool messageRemainingLengthIsComplete();
        bool determineMessageLength();
        void logIllegalRemainingLength();
        void logMessageSizeTooLarge();

    public:
        void begin(WiFiClient &wifiClient);
        bool matches(WiFiClient &wifiClient);
        bool readMessageData(MQTTMessage &message, bool &errorTerminateConnection);
        void resetMessageBuffer();
        bool write(const uint8_t *data, size_t size);
        bool hasSession();
        void connectTo(MQTTSession *session);
        void stop();
        bool wasDisconnected();
        MQTTSession *session();
        const IPAddress &ipAddress() const;
        uint16_t port() const;
        void updateConnectionDebug(DataModelStringLeaf &debug);
};

#endif
