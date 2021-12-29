#ifndef MQTT_PING_REQUEST_MESSAGE_H
#define MQTT_PING_REQUEST_MESSAGE_H

#include "MQTTMessage.h"

class MQTTPingRequestMessage : MQTTMessage {
    public:
        MQTTPingRequestMessage(MQTTMessage const &message);
        bool parse();
};

#endif
