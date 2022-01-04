#include <Arduino.h>

#include "Config.h"

const char wifiSSID[] = "NETWORK_NAME";
const char wifiPassword[] = "YOUR_PASSWORD";

const uint16_t century = 2000;

const char mmsi[] = "YOUR_MMSI";

// We use this ID to destinquish between multiple instances of this type controller within the
// vessel.
const char controllerID[] = "1";

const char *nmeaWiFiSourceIPAddress = "YOUR_VESPERS_ADDR";
extern const uint16_t nmeaWiFiSourceTCPPort = 39150 /* Or whatever port your gear is using */;
