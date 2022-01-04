#ifndef NMEA_WIFI_SOURCE_H
#define NMEA_WIFI_SOURCE_H

#include "NMEA/NMEASource.h"

#include "WiFiManager/WiFiManager.h"
#include "WiFiManager/WiFiManagerClient.h"

#include <WiFiNINA.h>

class NMEAWiFiSource : public NMEASource, public WiFiManagerClient {
    private:
        WiFiManager &wifiManager;
        WiFiClient client;
        bool clientConnected;

        void connect();

    public:
        NMEAWiFiSource(WiFiManager &);
        void begin();
        void service();
        virtual void wifiConnected() override;
        virtual void wifiDisconnected() override;
};

#endif
