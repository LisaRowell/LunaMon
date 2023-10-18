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

#ifndef MQTT_SESSION_H
#define MQTT_SESSION_H

class MQTTConnection;
class MQTTBroker;
class DataModelStringLeaf;

#include "DataModel/DataModelSubscriber.h"

#include "Util/PassiveTimer.h"

#include <etl/string.h>

#include <stdint.h>

//
// MQTTSession
//
// This class maintains information on a MQTT Client Session per the definition of the protocol.
// It's worth highlighting that in MQTT, a session's lifespan is not necessarily one-to-one with the
// lifespan of the TCP/IP connection that created it. This allows for clients with connection issues
// to reconnect without starting from scratch with the topics that they publish or subscribe to.
//

const unsigned maxMQTTClientIDLength = 23;

// The time, in seconds that we wait before tearing down a Session that's lost it's Connection and
// hasn't had a new one established.
const uint16_t unconnectedSessionTearDownTime = 120;

class MQTTSession : public DataModelSubscriber {
    private:
        MQTTBroker *broker;
        bool cleanSession;
        etl::string<maxMQTTClientIDLength> clientID;
        MQTTConnection *connection;
        uint16_t keepAliveTime;
        PassiveTimer keepAliveTimer;
        PassiveTimer tearDownTimer;

        void unsubscribeAll();

    public:
        bool isConnected() const;
        bool matches(const etl::istring &clientID) const;
        void begin(MQTTBroker *broker, bool cleanSession, const etl::istring &clientID,
                   MQTTConnection *connection, uint16_t keepAliveTime);
        void reconnect(bool newCleanSession, MQTTConnection *connection, uint16_t keepAliveTime);
        bool disconnect();
        void service();
        void resetKeepAliveTimer();
        virtual const etl::istring &name() const override;
        virtual void publish(const char *topic, const char *value, bool retainedValue) override;
        void updateSessionDebug(DataModelStringLeaf &debug);
};

#endif
