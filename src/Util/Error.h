#ifndef ERROR_H
#define ERROR_H

#include <Arduino.h>

extern void fatalError(String errorMsg) __attribute__((noreturn));
extern void errorExit() __attribute__((noreturn));

#endif
