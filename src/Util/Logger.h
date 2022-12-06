#ifndef LOGGER_H
#define LOGGER_H

#include "LoggableItem.h"

#include "DataModel/DataModel.h"

#include "MQTT/MQTTString.h"

#include <etl/string.h>
#include <etl/string_view.h>

#include <Arduino.h>

#include <stdint.h>

enum LoggerModule {
    LOGGER_MODULE_DATA_MODEL,
    LOGGER_MODULE_MQTT,
    LOGGER_MODULE_NMEA,
    LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE,
    LOGGER_MODULE_NMEA_WIFI_BRIDGE,
    LOGGER_MODULE_NMEA_WIFI,
    LOGGER_MODULE_UTIL,
    LOGGER_MODULE_WIFI_MANAGER,
    LOGGER_MODULE_STATS_MANAGER,
    LOGGER_MODULE_COUNT,
    LOGGER_MODULE_ANY = 0xff
};

enum LoggerLevel {
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_NOTIFY,
    LOGGER_LEVEL_ERROR
};
#define LOG_LEVEL_SHIFT 8
#define LOGGER_MODULE_MASK 0xff

#define LOG_SELECTOR(level, module) (((level) << LOG_LEVEL_SHIFT) | (module))
enum LogSelector {
    logDebugDataModel = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_DATA_MODEL),
    logDebugMQTT = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_MQTT),
    logDebugNMEA = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA),
    logDebugNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logDebugNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_WIFI),
    logDebugUtil = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_UTIL),
    logDebugWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_WIFI_MANAGER),
    logDebugStatsManager = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STATS_MANAGER),
    logWarning = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_ANY),
    logNotify = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_ANY),
    logError = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_ANY)
};

enum LogBase {
    Dec,
    Hex
};

struct EndOfLine {};
const EndOfLine eol = EndOfLine();

class Logger {
    private:
        LoggerLevel logLevel;
        LoggerLevel lineLevel;
        bool outputCurrentLine;
        bool moduleDebugFlags[LOGGER_MODULE_COUNT];

        LogBase base;
        Stream &console;
        uint8_t errorsSetInDataModel;
        char errorLine[maxErrorLength];
        unsigned errorLinePos;
        // Flag used to make sure that we don't try to set an error in the DataModel that occured
        // while trying to set an error in the DataModel.
        bool inLogger;

        void logString(const char *string);
        void logCharacter(char character);
        void addErrorLineToDebugs();
        void scrollUpDebugs();

    public:
        Logger(LoggerLevel level, Stream &console);
        void setLevel(LoggerLevel level);
        void enableModuleDebug(LoggerModule module);
        void disableModuleDebug(LoggerModule module);
        Logger & operator << (const LogSelector level);
        Logger & operator << (const LogBase base);
        Logger & operator << (char character);
        Logger & operator << (const char *string);
        Logger & operator << (const etl::istring &string);
        Logger & operator << (const etl::string_view &stringView);
        Logger & operator << (uint8_t value);
        Logger & operator << (uint16_t value);
        Logger & operator << (uint32_t value);
        Logger & operator << (unsigned value);
        Logger & operator << (int16_t value);
        Logger & operator << (int value);
        Logger & operator << (bool value);
        // The following two are here instead of being LoggableItems because they are used in data
        // streams which would be messed up by having a virtual function pointer thrown in there.
        Logger & operator << (const IPAddress &addr);
        Logger & operator << (const MQTTString &string);
        Logger & operator << (const LoggableItem &item);
        Logger & operator << (const EndOfLine &eol);
};

extern Logger logger;

#endif
