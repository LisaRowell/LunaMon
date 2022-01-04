#ifndef WIFI_MANAGER_CLIENT_H
#define WIFI_MANAGER_CLIENT_H

class WiFiManagerClient {
    public:
        virtual void wifiConnected() = 0;
        virtual void wifiDisconnected() = 0;
};

#endif
