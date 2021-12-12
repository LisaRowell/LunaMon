#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

#include "MQTT/MQTTString.h"
#include "NMEA/NMEATalker.h"

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

    public:
        Logger(LogLevel level, Stream &console);
        Logger & operator << (const LogLevel level);
        Logger & operator << (const LogBase base);
        Logger & operator << (const char *string);
        Logger & operator << (const String &string);
        Logger & operator << (uint8_t value);
        Logger & operator << (uint16_t value);
        Logger & operator << (uint32_t value);
        Logger & operator << (unsigned value);
        Logger & operator << (int value);
        Logger & operator << (const IPAddress &addr);
        Logger & operator << (const MQTTString &string);
        Logger & operator << (enum NMEATalker talker);
        Logger & operator << (const EndOfLine &eol);
};

extern Logger logger;

#endif