#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H

#include <WiFiNINA.h>

#include "MQTTMessage.h"

class MQTTSession;

class MQTTConnection {
  private:
    WiFiClient wifiClient;
    MQTTSession *mqttSession;

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
};

#endif