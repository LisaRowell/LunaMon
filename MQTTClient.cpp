#include <Arduino.h>

#include "MQTTClient.h"


void MQTTClient::service() {
  // Arduino server implementation is a bit wonky, not providing a clean
  // way of getting what new clients are connected. Instead there's an
  // available routine that returns a client that has data available to
  // read. We do client reading based off that call and in this method
  // we just do house keeping like checking for broken connections.

  
}