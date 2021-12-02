#ifndef WIFI_MANAGER_CLIENT_H
#define WIFI_MANAGER_CLIENT_H

class WiFiManagerClient {
  private:
  public:
    virtual void wifiConnected();
    virtual void wifiDisconnected();
};

#endif