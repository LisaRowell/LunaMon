#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

#include "LoggableItem.h"

#include "DataModel/DataModel.h"

#include "MQTT/MQTTString.h"

enum LogLevel {
    logNone,
    logDebug,
    logWarning,
    logNotify,
    logError
};

enum LogBase {
    Dec,
    Hex
};

struct EndOfLine {};
const EndOfLine eol = EndOfLine();

class Logger {
    private:
        LogLevel logLevel;
        LogLevel lineLevel;
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
        Logger(LogLevel level, Stream &console);
        Logger & operator << (const LogLevel level);
        Logger & operator << (const LogBase base);
        Logger & operator << (char character);
        Logger & operator << (const char *string);
        Logger & operator << (const String &string);
        Logger & operator << (uint8_t value);
        Logger & operator << (uint16_t value);
        Logger & operator << (uint32_t value);
        Logger & operator << (unsigned value);
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
