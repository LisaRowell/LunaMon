#ifndef IP_ADDRESS_TOOLS_H
#define IP_ADDRESS_TOOLS_H

#include <IPAddress.h>

const size_t maxIPAddressTextLength = 15 + 1;

void ipAddressToStr(char *string, const arduino::IPAddress &ipAddr);

#endif
