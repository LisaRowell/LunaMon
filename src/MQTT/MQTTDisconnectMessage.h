#ifndef MQTT_DISCONNECT_MESSAGE_H
#define MQTT_DISCONNECT_MESSAGE_H

#include "MQTTMessage.h"

class MQTTDisconnectMessage : MQTTMessage {
    public:
        MQTTDisconnectMessage(MQTTMessage const &message);
        bool parse();
};

#endif