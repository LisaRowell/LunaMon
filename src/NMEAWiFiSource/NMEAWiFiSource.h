#ifndef NMEA_WIFI_SOURCE_H
#define NMEA_WIFI_SOURCE_H

class WiFiManager;
class DataModelLeaf;
class StatsManager;

#include "NMEA/NMEASource.h"

#include "WiFiManager/WiFiManagerClient.h"

#include <WiFiNINA.h>

class NMEAWiFiSource : public NMEASource, public WiFiManagerClient {
    private:
        WiFiManager &wifiManager;
        WiFiClient client;
        bool clientConnected;

        void connect();

    public:
        NMEAWiFiSource(WiFiManager &wifiManager, DataModelLeaf &messageCountDataModelLeaf,
                       DataModelLeaf &messageRateDataModelLeaf, StatsManager &statsManager);
        void begin();
        void service();
        virtual void wifiConnected() override;
        virtual void wifiDisconnected() override;
};

#endif
