#include <Arduino.h>

#include "IPAddressTools.h"

void ipAddressToStr(char *string, IPAddress ipAddr) {
    snprintf(string, maxIPAddressTextLength, "%u.%u.%u.%u", ipAddr[0], ipAddr[1], ipAddr[2],
             ipAddr[3]);
}
