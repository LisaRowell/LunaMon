#ifndef LOGGABLE_ITEM_H
#define LOGGABLE_ITEM_H

class Logger;

class LoggableItem {
    public:
        virtual void log(Logger &logger) const = 0;
};

#endif
