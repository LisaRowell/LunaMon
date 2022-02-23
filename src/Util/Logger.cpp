#include "Logger.h"
#include "LoggableItem.h"
#include "Error.h"

#include "MQTT/MQTTString.h"

#include <etl/string.h>
#include <Arduino.h>

using etl::istring;

Logger logger(LOGGER_LEVEL_WARNING, Serial);

Logger::Logger(LoggerLevel level, Stream &console)
    : logLevel(level), lineLevel(LOGGER_LEVEL_ERROR), outputCurrentLine(false), base(Dec),
      console(console), errorsSetInDataModel(0), errorLinePos(0), inLogger(false) {
    errorLine[0] = 0;

    unsigned moduleIndex;
    for (moduleIndex = 0; moduleIndex < LOGGER_MODULE_COUNT; moduleIndex++) {
        moduleDebugFlags[moduleIndex] = false;
    }
}

void Logger::setLevel(LoggerLevel level) {
    logLevel = level;
}

void Logger::enableModuleDebug(LoggerModule module) {
    moduleDebugFlags[module] = true;
}

void Logger::disableModuleDebug(LoggerModule module) {
    moduleDebugFlags[module] = false;
}

Logger & Logger::operator << (const LogSelector logSelector) {
    lineLevel = (LoggerLevel)((uint16_t)logSelector >> LOG_LEVEL_SHIFT);
    if (lineLevel >= logLevel) {
        if (lineLevel == LOGGER_LEVEL_DEBUG) {
            const enum LoggerModule module =
                (LoggerModule)((uint16_t)logSelector & LOGGER_MODULE_MASK);
            outputCurrentLine = moduleDebugFlags[module];
        } else {
            outputCurrentLine = true;
        }
    } else {
        outputCurrentLine = false;
    }

    return *this;
}

Logger & Logger::operator << (const LogBase base) {
    Logger::base = base;

    return *this;
}

Logger & Logger::operator << (char character) {
    if (outputCurrentLine) {
        logCharacter(character);
    }

    return *this;
}

Logger & Logger::operator << (const char *string) {
    if (outputCurrentLine) {
        if (string != NULL) {
            logString(string);
        } else {
            logString("(nil)");
        }
    }

    return *this;
}

Logger & Logger::operator << (const istring &string) {
    if (outputCurrentLine) {
       logString(string.data());
    }

    return *this;
}

Logger & Logger::operator << (const String &string) {
    if (outputCurrentLine) {
       logString(string.c_str());
    }

    return *this;
}

Logger & Logger::operator << (uint8_t value) {
    if (outputCurrentLine) {
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
    if (outputCurrentLine) {
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
    if (outputCurrentLine) {
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
    if (outputCurrentLine) {
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

Logger & Logger::operator << (int16_t value) {
    if (outputCurrentLine) {
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

Logger & Logger::operator << (int value) {
    if (outputCurrentLine) {
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
    if (outputCurrentLine) {
        if (value) {
            logString("true");
        } else {
            logString("false");
        }
    }

    return *this;
}

Logger & Logger::operator << (const IPAddress &addr) {
    if (outputCurrentLine) {
        char ipAddressStr[maxIPAddressTextLength];
        ipAddressToStr(ipAddressStr, addr);
        logString(ipAddressStr);
    }

    return *this;
}

Logger & Logger::operator << (const MQTTString &string) {
    if (outputCurrentLine) {
        const uint16_t length = string.length();

        unsigned pos;
        for (pos = 0; pos < length; pos++) {
            logCharacter(string.characterData[pos]);
        }
    }

    return *this;
}

Logger & Logger::operator << (const LoggableItem &item) {
    if (outputCurrentLine) {
        item.log(*this);
    }

    return *this;
}

Logger & Logger::operator << (const EndOfLine &eol) {
    base = Dec;
 
    if (outputCurrentLine) {
        console.println();

        if (lineLevel >= LOGGER_LEVEL_WARNING) {
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

    if (lineLevel >= LOGGER_LEVEL_WARNING && !inLogger) {
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

    if (lineLevel >= LOGGER_LEVEL_WARNING && !inLogger) {
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
