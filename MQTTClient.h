#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFiNINA.h>


//
// MQTTClient
//
// This class maintains information on a MQTT Client per the definition of the
// protocol. It's worth highlighting that in MQTT, a client's lifespan is not
// necessarily one-to-one with the lifespan of the TCP/IP connection that
// created it. This allows for clients with connection issues to reconnect
// without starting from scratch with the topics that they publish or subscribe
// to.
//
class MQTTClient {
  private:
    bool connectionValid;
    // While the Arduino WiFiClient class maintains the remote TCP/IP address
    // of the client, it seems to get moshed when the client disconnects,
    // preventing us from using it in log messages. To get around this, we
    // maintain our own copy.
    IPAddress ipAddress;
    uint16_t tcpPort;
    WiFiClient connection;

  public:
    void service();
};

#endif