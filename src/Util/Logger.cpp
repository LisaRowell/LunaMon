#include <Arduino.h>

#include "Logger.h"
#include "MQTT/MQTTString.h"
#include "Error.h"

Logger logger(logDebug, Serial);

Logger::Logger(LogLevel level, Stream &console)
    : logLevel(level), base(Dec), console(console) {
}

Logger & Logger::operator << (const LogLevel level) {
    logLevel = lineLevel;

    return *this;
}

Logger & Logger::operator << (const LogBase base) {
    Logger::base = base;

    return *this;
}

Logger & Logger::operator << (const char *string) {
    if (lineLevel >= logLevel) {
        console.print(string);
    }

    return *this;
}

Logger & Logger::operator << (const String &string) {
    if (lineLevel >= logLevel) {
       console.print(string);
    }

    return *this;
}

Logger & Logger::operator << (uint8_t value) {
    if (lineLevel >= logLevel) {
        switch (base) {
            case Dec:
                console.print(value);
                break;

            case Hex:
                console.print(value, HEX);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (uint16_t value) {
    if (lineLevel >= logLevel) {
        switch (base) {
            case Dec:
                console.print(value);
                break;

            case Hex:
                console.print(value, HEX);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (uint32_t value) {
    if (lineLevel >= logLevel) {
        switch (base) {
            case Dec:
                console.print(value);
                break;

            case Hex:
                console.print(value, HEX);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (int value) {
    if (lineLevel >= logLevel) {
        switch (base) {
            case Dec:
                console.print(value);
                break;

            case Hex:
                console.print(value, HEX);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (const IPAddress &addr) {
    if (lineLevel >= logLevel) {
        console.print(addr);
    }

    return *this;
}

Logger & Logger::operator << (const MQTTString &string) {
    if (lineLevel >= logLevel) {
        const uint16_t length = string.length();

        unsigned pos;
        for (pos = 0; pos < length; pos++) {
            console.print(string.characterData[pos]);
        }
    }

    return *this;
}

Logger & Logger::operator << (enum NMEATalker talker) {
    if (lineLevel >= logLevel) {
        console.print(nmeaTalkerName(talker));
    }

    return *this;
}

Logger & Logger::operator << (const EndOfLine &eol) {
    base = Dec;
 
    if (lineLevel >= logLevel) {
        console.println();
    }

    return *this;
}