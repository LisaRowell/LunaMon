#include "IPAddressTools.h"

#include <IPAddress.h>
#include <stdio.h>

void ipAddressToStr(char *string, const arduino::IPAddress &ipAddr) {
    snprintf(string, maxIPAddressTextLength, "%u.%u.%u.%u", ipAddr[0], ipAddr[1], ipAddr[2],
             ipAddr[3]);
}
