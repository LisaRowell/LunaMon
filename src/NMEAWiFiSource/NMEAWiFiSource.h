#ifndef NMEA_WIFI_SOURCE_H
#define NMEA_WIFI_SOURCE_H

class WiFiManager;
class DataModelLeaf;
class DataModelBoolLeaf;
class StatsManager;

#include "NMEA/NMEASource.h"

#include "WiFiManager/WiFiManagerClient.h"

#include <WiFiNINA.h>

class NMEAWiFiSource : public NMEASource, public WiFiManagerClient {
    private:
        WiFiManager &wifiManager;
        WiFiClient client;
        bool clientConnected;
        DataModelBoolLeaf &connectionStatusLeaf;

        void connect();

    public:
        NMEAWiFiSource(WiFiManager &wifiManager, DataModelLeaf &messageCountDataModelLeaf,
                       DataModelLeaf &messageRateDataModelLeaf,
                       DataModelBoolLeaf &connectionStatusLeaf, StatsManager &statsManager);
        void begin();
        void service();
        virtual void wifiConnected() override;
        virtual void wifiDisconnected() override;
};

#endif
