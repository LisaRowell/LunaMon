#include "IPAddressTools.h"

#include <IPAddress.h>
#include <stdio.h>

using arduino::IPAddress;

void ipAddressToStr(char *string, const IPAddress &ipAddr) {
    snprintf(string, maxIPAddressTextLength, "%u.%u.%u.%u", ipAddr[0], ipAddr[1], ipAddr[2],
             ipAddr[3]);
}
