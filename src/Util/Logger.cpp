#include <Arduino.h>

#include "Logger.h"
#include "MQTT/MQTTString.h"
#include "Error.h"

Logger logger(logDebug, Serial);

Logger::Logger(LogLevel level, Stream &console)
    : logLevel(level), lineLevel(logError), base(Dec), console(console), errorsSetInDataModel(0),
      errorLinePos(0), inLogger(false) {
    errorLine[0] = 0;
}

Logger & Logger::operator << (const LogLevel level) {
    lineLevel = level;

    return *this;
}

Logger & Logger::operator << (const LogBase base) {
    Logger::base = base;

    return *this;
}

Logger & Logger::operator << (const char *string) {
    if (lineLevel >= logLevel) {
        if (string != NULL) {
            logString(string);
        } else {
            logString("(nil)");
        }
    }

    return *this;
}

Logger & Logger::operator << (const String &string) {
    if (lineLevel >= logLevel) {
       logString(string.c_str());
    }

    return *this;
}

Logger & Logger::operator << (uint8_t value) {
    if (lineLevel >= logLevel) {
        switch (base) {
            case Dec:
                char decimalStr[4];
                snprintf(decimalStr, 4, "%u", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[5];
                snprintf(hexStr, 5, "0x%0x", value);
                logString(hexStr);
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
                char decimalStr[6];
                snprintf(decimalStr, 6, "%u", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[7];
                snprintf(hexStr, 7, "0x%0x", value);
                logString(hexStr);
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
                char decimalStr[11];
                snprintf(decimalStr, 11, "%lu", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0lx", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (unsigned value) {
    if (lineLevel >= logLevel) {
        switch (base) {
            case Dec:
                char decimalStr[11];
                snprintf(decimalStr, 11, "%u", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0x", value);
                logString(hexStr);
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
                char decimalStr[12];
                snprintf(decimalStr, 12, "%d", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0x", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (bool value) {
    if (lineLevel >= logLevel) {
        if (value) {
            logString("true");
        } else {
            logString("false");
        }
    }

    return *this;
}

Logger & Logger::operator << (const IPAddress &addr) {
    if (lineLevel >= logLevel) {
        char ipAddressStr[maxIPAddressTextLength];
        ipAddressToStr(ipAddressStr, addr);
        logString(ipAddressStr);
    }

    return *this;
}

Logger & Logger::operator << (const MQTTString &string) {
    if (lineLevel >= logLevel) {
        const uint16_t length = string.length();

        unsigned pos;
        for (pos = 0; pos < length; pos++) {
            logCharacter(string.characterData[pos]);
        }
    }

    return *this;
}

Logger & Logger::operator << (const NMEATalker &talker) {
    if (lineLevel >= logLevel) {
        logString(talker.name());
    }

    return *this;
}

Logger & Logger::operator << (const EndOfLine &eol) {
    base = Dec;
 
    if (lineLevel >= logLevel) {
        console.println();

        if (lineLevel >= logWarning) {
            addErrorLineToDebugs();
        }
    }

    if (!inLogger) {
        errorLine[0] = 0;
        errorLinePos = 0;
    }

    return *this;
}

void Logger::logString(const char *string) {
    console.print(string);

    if (lineLevel >= logWarning && !inLogger) {
        unsigned strPos;
        for (strPos = 0;
             errorLinePos < maxErrorLength - 1 && string[strPos];
             strPos++, errorLinePos++) {
            errorLine[errorLinePos] = string[strPos];
        }
        errorLine[errorLinePos] = 0;
    }
}

void Logger::logCharacter(char character) {
    console.print(character);

    if (lineLevel >= logWarning && !inLogger) {
        errorLine[errorLinePos] = character;
        errorLinePos++;
        errorLine[errorLinePos] = 0;
    }
}

void Logger::addErrorLineToDebugs() {
    if (!inLogger) {
        inLogger = true;

        if (errorsSetInDataModel == errorDebugSlots) {
            scrollUpDebugs();
        }

        DataModelStringLeaf *errorDebug = errorDebugs[errorsSetInDataModel];
        *errorDebug = errorLine;
        errorsSetInDataModel++;

        inLogger = false;
    }
}

void Logger::scrollUpDebugs() {
    unsigned slot;
    for (slot = 0; slot < errorDebugSlots - 1; slot++) {
        DataModelStringLeaf *toErrorDebug = errorDebugs[slot];
        DataModelStringLeaf *fromErrorDebug = errorDebugs[slot + 1];
        *toErrorDebug = *fromErrorDebug;
    }
    errorsSetInDataModel--;
}
